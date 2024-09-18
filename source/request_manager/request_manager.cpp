// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "include/request_manager.h"

extern TLogger logger;

namespace NRequest {
    nlohmann::json configJson;
}


void NRequest::InitConfig() {
    std::ifstream configStream("../configuration.json");
    
    logger << TLevel::Info << "began openning the file with configuration\n";

    if (!configStream.is_open()) {
        logger << TLevel::Fatal << "~ InitConfig() error: ";
        logger << "file with configuration of the program wasn't opened\n\n";
        throw TRequestException("can't open the configuration.json", 402);
    }

    logger << TLevel::Info << "deserealization of the configuration has begun\n";
    
    try {
        configStream >> configJson;

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Fatal;
        logger << "~ InitConfig() error: during the deserealization of the configuration.json:\n\t";
        logger << parseExcp.what() << "\n\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);
    }

    logger << TLevel::Info << "deserealization of the configuration finished successfully\n\n";
}


void NRequest::TWeatherModule::InitWeatherEndpoints() {
    logger << TLevel::Info << "began receiving the ip from resolver for 'api.openweathermap.org'\n";

    try {
        boost::asio::ip::tcp::resolver resolver(TWeatherModule::RequestService);
        boost::asio::ip::tcp::resolver::query query("api.openweathermap.org", "80");
        boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        
        logger << TLevel::Info << "got ip for domen 'api.openweathermap.org' from resolver\n";
       
        TWeatherModule::EpOpenWeatherRequest = std::move(*iter);

    } catch (boost::system::system_error& excp) {
        logger << TLevel::Fatal << "~ InitWeatherEndpoints() error: " << excp.what() << "\n\n";
        throw TRequestException(excp.what(), 501);
    }
}


void NRequest::TWeatherModule::InitWeatherApiKey() {
    logger << TLevel::Info << "began the initializing the ApiKey for the 'api.openweathermap.org'\n";

    try {
        std::string openWeatherKey = configJson.at("API_KEY").at("api.openweathermap.org").dump();
        TWeatherModule::OpenWeatherApiKey = std::move(openWeatherKey.substr(1, openWeatherKey.size() - 2));

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Fatal;
        logger << "~ InitWeatherApiKey() error: during initializing the Api Keys: ";
        logger << typeExcp.what() << "\n\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Fatal;
        logger << "~ InitWeatherApiKey() error: during initializing the Api Keys: ";
        logger << parseExcp.what() << "\n\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);

    } catch (std::exception& excp) {
        logger << TLevel::Fatal;
        logger << "~ InitWeatherApiKey() error: during initializing the Api Keys: ";
        logger << excp.what() << "\n";
        throw TRequestException(excp.what(), 503);
    }

    logger << TLevel::Info << "initializing the ApiKey for the 'api.openweathermap.org' finished correctly\n\n";
}


void NRequest::ConfigureRequestService() {
    InitConfig();
    TWeatherModule::InitWeatherEndpoints();
    TWeatherModule::InitWeatherApiKey();
}


NRequest::TWeatherModule::TWeatherModule()
    : SocketWeath(RequestService)
{
}


std::string NRequest::TWeatherModuleConverter::ConvertPascalsToMmHg(std::string pressure) const {
    double press;
    std::istringstream pressIn(std::move(pressure));
    std::ostringstream pressOut;

    pressIn >> press;
    press /= 1.333;
    pressOut << press;

    return pressOut.str();
}


std::string NRequest::TWeatherModuleConverter::ConvertKelvinsToCelsus(std::string temp) const {
    double tempNum;
    std::istringstream tempIn(std::move(temp));
    std::ostringstream tempOut;

    tempIn >> tempNum;
    tempNum -= 273.;
    tempOut << tempNum;

    return tempOut.str();
}


std::string NRequest::TWeatherModuleConverter::ConvertToUrlView(const std::string& obj) const {
    std::string urlObjView;
    
    for (const auto& elem : obj) {
        if (elem == ' ') {
            urlObjView += "+";
        } else {
            urlObjView.push_back(elem);
        }
    }

    return urlObjView;
}


std::string NRequest::TWeatherModuleConverter::ConvertWeatherJsonToWeatherTemplate(const nlohmann::json& weathJson) const {
    logger << TLevel::Debug << "the creation of the weather description for weather template has begun\n";

    std::string weatherDesc;

    try {
        weatherDesc += "<br>- Basic Weather Description: ";
        weatherDesc += weathJson.at("weather").at(0).at("description").dump() + "</br>";

        weatherDesc += "<br>- Temperature (C): ";
        weatherDesc += ConvertKelvinsToCelsus(weathJson.at("main").at("temp").dump()) + "</br>";

        weatherDesc += "<br>- Temperature \"feels_like\" (C): ";
        weatherDesc += ConvertKelvinsToCelsus(weathJson.at("main").at("feels_like").dump()) + "</br>";

        weatherDesc += "<br>- Pressure (mmHg): ";
        weatherDesc += ConvertPascalsToMmHg(weathJson.at("main").at("pressure").dump()) + "</br>";

        weatherDesc += "<br>- Wind (speed/gust m/s): ";
        weatherDesc += weathJson.at("wind").at("speed").dump() + "/";

        if (weathJson.at("wind").contains("gust")) {
            weatherDesc += weathJson.at("wind").at("gust").dump() + "</br>";
        } else {
            weatherDesc += "0</br>";
        }

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Error << "~ ConvertWeatherJsonToWeatherTemplate() error: " << typeExcp.what();
        logger << "\n\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Error << "~ ConvertWeatherJsonToWeatherTemplate() error: " << parseExcp.what();
        logger << "\n\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);
    }

    logger << TLevel::Debug << "the weather description for weather template was successfully made\n\n";

    return weatherDesc;
}


std::string NRequest::TWeatherModule::GetWeatherJson() {
    std::string weathBuff(4096, '\0');

    logger << TLevel::Debug << "began sending the request to 'api.openweathermap.org' ";
    logger << "(weather-service) for receiving the weather of the city '" << City << "'\n";

    try {
        std::string url = "https://api.openweathermap.org/data/2.5/weather?q=";
        std::ostringstream requestStream;

        url += Converter.ConvertToUrlView(City) + ",RU";
        url += "&appid=";
        url += OpenWeatherApiKey;

        SocketWeath.connect(TWeatherModule::EpOpenWeatherRequest);

        requestStream << "GET " << url << "\r\n";
        requestStream << "Host: api.openweathermap.org\r\n";
        requestStream << "APPID: " << OpenWeatherApiKey << "\r\n";
        requestStream << "\r\n\r\n";

        SocketWeath.write_some(boost::asio::buffer(requestStream.str()));
        SocketWeath.read_some(boost::asio::buffer(weathBuff));

        SocketWeath.close();

    } catch (boost::system::system_error& excp) {
        SocketWeath.close();
        logger << TLevel::Fatal << "~ GetWeatherJson() error: " << excp.what() << "\n\n";
        throw TRequestException(excp.what(), 501);

    } catch (TRequestException& excp) {
        SocketWeath.close();
        throw;

    } catch (std::exception& excp) {
        SocketWeath.close();
        logger << TLevel::Error << "~ GetWeatherJson() error: " << excp.what() << "\n\n";
        throw TRequestException(excp.what(), 503);

    } catch (...) {
        SocketWeath.close();
        logger << TLevel::Error << "~ GetWeatherJson() error: unpredictable error was generated" << "\n\n";
        throw;
    }

    logger << TLevel::Debug << "the weather json from 'api.openweathermap.org for the city '";
    logger << City << "' was correcly recieved\n\n";

    return weathBuff;
}


std::string NRequest::TWeatherModule::TryGetWeatherTemplate(const std::string& city) {
     if (city.empty()) {
        logger << TLevel::Error << "~ TryGetWeatherTemplate() error: the City is empty: incorrect data\n\n";
        throw TRequestException("the member City is empty", 401);    
    }

    City = city;
    WeatherDesc.clear();

    logger << TLevel::Debug << "the process of receiving the weather description ";
    logger << "(request_manager-service) for the city '" << City << "' has begun\n";
    
    try {
        nlohmann::json weathJson = nlohmann::json::parse(GetWeatherJson());

        if (weathJson.at("cod").dump() != "200") {
            logger << TLevel::Error << "~ TryGetWeatherTemplate() error: ";
            logger << "the answer from the server IS NOT 200 OK\n";
            throw TRequestException("the code status for the weather description isn't 200 (not OK)",
                404);
        }
        WeatherDesc = std::move(Converter.ConvertWeatherJsonToWeatherTemplate(weathJson));

    } catch (nlohmann::json::parse_error& parseExcp) {
        WeatherDesc.clear();
        City.clear();
        logger << TLevel::Error << "~ TryGetWeatherTemplate() error: " << parseExcp.what() << "\n";
        logger << "\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);

    } catch (nlohmann::json::type_error& typeExcp) {
        WeatherDesc.clear();
        City.clear();
        logger << TLevel::Error << "~ TryGetWeatherTemplate() error: " << typeExcp.what() << "\n";
        logger << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (...) {
        WeatherDesc.clear();
        City.clear();
        throw;
    }

    logger << TLevel::Debug << "the weather description was successfully made for the city '";
    logger << City << "':\n" << WeatherDesc << "\n\n";

    return WeatherDesc;
}


std::string NRequest::TRequestManager::GetWeatherTemplate(const std::string& city) {
    logger << TLevel::Debug << "the request_manager-service started receiving\n";

    std::string weathDesc;

    for (bool noThrow = false; !noThrow; ) {
        try {
            weathDesc = std::move(WeatherModule.TryGetWeatherTemplate(city));
            noThrow = true;

        } catch (NRequest::TRequestException& excp) {
            if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
                throw;
            }

        } catch (...) {
            throw;
        }
    }
    
    logger << TLevel::Debug << "the request_manager-service stopped receiving\n";

    return weathDesc;
}
