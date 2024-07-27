#include "include/request_manager.h"

extern TLogger logger;

namespace NRequest {
    nlohmann::json configJson;
}

namespace NWeather {
    extern std::mutex serviceMut;
}


void NRequest::InitConfig() {
    std::ifstream configStream("../configuration.json");
    
    logger << TLevel::Info << "began openning the file with configuration\n";

    if (!configStream.is_open()) {
        logger << TLevel::Fatal << "~ InitConfig() error: ";
        logger << "file with configuration of the programme wasn't opened\n\n";
        throw TRequestException("can't open the configuration.json", 402);
    }

    try {
        logger << TLevel::Info << "deserealization of the configuration has begun\n";
        configStream >> configJson;

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Fatal;
        logger << "~ InitConfig() error: during the deserealization of the configuration.json:\n\t";
        logger << parseExcp.what() << "\n\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);
    }

    logger << TLevel::Info << "deserealization of the configuration finished successfully\n\n";
}


void NRequest::InitNetParams() {
    logger << TLevel::Info << "began receiving the ip from resolver for 'api.openweathermap.org'\n";

    try {
        boost::asio::ip::tcp::resolver resolver(TRequestManager::RequestService);
        boost::asio::ip::tcp::resolver::query query("api.openweathermap.org", "80");
        boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        
        logger << TLevel::Info << "got ip for domen 'api.openweathermap.org' from resolver\n";
       
        TRequestManager::EpWeatherRequest = std::move(*iter);

    } catch (boost::system::system_error& excp) {
        logger << TLevel::Fatal << "~ InitNetParams() error: " << excp.what() << "\n\n";
        throw TRequestException(excp.what(), 501);
    }
}


void NRequest::TRequestManager::InitApiKey() {
    logger << TLevel::Info << "began initializing the ApiKey for the 'api.openweathermap.org'\n";

    try {
        std::string apiKeyStr = configJson.at("API_KEY").dump();
        TRequestManager::ApiKey = std::move(apiKeyStr.substr(1, apiKeyStr.size() - 2));

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Fatal;
        logger << "~ InitApiKey() error: during initializing the ApiKey: " << typeExcp.what() << "\n";
        logger << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Fatal;
        logger << "~ InitApiKey() error: during initializing the ApiKey: " << parseExcp.what() << "\n";
        logger << "\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);

    } catch (std::exception& excp) {
        logger << TLevel::Fatal;
        logger << "~ InitApiKey() error: during initializing the ApiKey: " << excp.what() << "\n";
        throw TRequestException(excp.what(), 503);
    }

    logger << TLevel::Info;
    logger << "initializing the ApiKey for the 'api.openweathermap.org' finished correctly\n\n";
}


void NRequest::ConfigureRequestService() {
    InitConfig();
    InitNetParams();
    TRequestManager::InitApiKey();
}


NRequest::TRequestManager::TRequestManager()
    : SocketWeath(RequestService)
{
}


NRequest::TRequestManager::TRequestManager(const std::string& city)
    : SocketWeath(RequestService)
    , City(city)
{
}


std::string NRequest::TRequestManager::GetMmHg(std::string pressure) const {
    double press;
    std::istringstream pressIn(std::move(pressure));
    std::ostringstream pressOut;

    pressIn >> press;
    press /= 1.333;
    pressOut << press;

    return pressOut.str();
}


std::string NRequest::TRequestManager::GetCelsus(std::string temp) const {
    double tempNum;
    std::istringstream tempIn(std::move(temp));
    std::ostringstream tempOut;

    tempIn >> tempNum;
    tempNum -= 273.;
    tempOut << tempNum;

    return tempOut.str();
}


std::string NRequest::TRequestManager::GetUrlCityView() {
    std::string urlCityView;
    
    for (const auto& elem : City) {
        if (elem == ' ') {
            urlCityView += "+";
        } else {
            urlCityView.push_back(elem);
        }
    }

    return urlCityView;
}


void NRequest::TRequestManager::SetWeatherDesc(const nlohmann::json& weathJson) {
    logger << TLevel::Debug << "the creation of the weather description has begun\n";

    WeatherDesc.clear();

    try {
        WeatherDesc += "<br>- Basic Weather Description: ";
        WeatherDesc += weathJson.at("weather").at(0).at("description").dump() + "</br>";

        WeatherDesc += "<br>- Temperature (C): ";
        WeatherDesc += GetCelsus(weathJson.at("main").at("temp").dump()) + "</br>";

        WeatherDesc += "<br>- Temperature \"feels_like\" (C): ";
        WeatherDesc += GetCelsus(weathJson.at("main").at("feels_like").dump()) + "</br>";

        WeatherDesc += "<br>- Pressure (mmHg): ";
        WeatherDesc += GetMmHg(weathJson.at("main").at("pressure").dump()) + "</br>";

        WeatherDesc += "<br>- Wind (speed/gust m/s): ";
        WeatherDesc += weathJson.at("wind").at("speed").dump() + "/";

        if (weathJson.at("wind").contains("gust")) {
            WeatherDesc += weathJson.at("wind").at("gust").dump() + "</br>";
        } else {
            WeatherDesc += "0</br>";
        }

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Error << "~ SetWeatherDesc() error: " << typeExcp.what() << "\n";
        logger << TLevel::Error << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Error << "~ SetWeatherDesc() error: " << parseExcp.what() << "\n";
        logger << TLevel::Error << "\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);
    }

    logger << TLevel::Debug << "the weather description was successfully made\n\n";
}


std::string NRequest::TRequestManager::GetWeatherJson() {
    std::string weathBuff(4096, '\0');

    logger << TLevel::Debug << "began sending the request to 'api.openweathermap.org' ";
    logger << "(weather-service) for receiving the weather of the city '" << City << "'\n";

    try {
        std::string url = "https://api.openweathermap.org/data/2.5/weather?q=";
        std::ostringstream requestStream;

        url += GetUrlCityView() + ",RU";
        url += "&appid=";
        url += ApiKey;

        SocketWeath.connect(TRequestManager::EpWeatherRequest);

        requestStream << "GET " << url << "\r\n";
        requestStream << "Host: api.openweathermap.org\r\n";
        requestStream << "APPID: " << ApiKey << "\r\n";
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


std::string NRequest::TRequestManager::GetWeatherService(const std::string& city) {
     if (city.empty()) {
        logger << TLevel::Error << "~ GetWeatherService() error: the City is empty: incorrect data\n\n";
        throw TRequestException("the member City is empty", 401);    
    }

    City = city;
    WeatherDesc.clear();

    logger << TLevel::Debug << "the process of receiving the weather description ";
    logger << "(request_manager-service) for the city '" << City << "' has begun\n";
    
    try {
        nlohmann::json weathJson = nlohmann::json::parse(GetWeatherJson());

        if (weathJson.at("cod").dump() != "200") {
            logger << TLevel::Error << "~ GetWeatherService() error: ";
            logger << "the answer from the server IS NOT 200 OK\n";
            throw TRequestException("the code status for the weather description isn't 200 (not OK)",
                404);
        }
        SetWeatherDesc(weathJson);

    } catch (nlohmann::json::parse_error& parseExcp) {
        WeatherDesc.clear();
        logger << TLevel::Error << "~ GetWeatherService() error: " << parseExcp.what() << "\n";
        logger << TLevel::Error << "\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);

    } catch (nlohmann::json::type_error& typeExcp) {
        WeatherDesc.clear();
        logger << TLevel::Error << "~ GetWeatherService() error: " << typeExcp.what() << "\n";
        logger << TLevel::Error << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (...) {
        WeatherDesc.clear();
        throw;
    }

    logger << TLevel::Debug << "the weather description was successfully made for the city '";
    logger << City << "':\n" << WeatherDesc << "\n\n";

    return WeatherDesc;
}


std::string NRequest::TRequestManager::GetWeatherService() {
    if (!City.empty()) {
        return GetWeatherService(City);

    } else {
        logger << TLevel::Error << "~ GetWeatherService() error: the City is empty: incorrect data\n\n";
        throw TRequestException("the member City is empty", 401);
    }
}


std::string NRequest::TRequestManager::GetWeather(const std::string& city) {
    logger << TLevel::Debug << "the request_manager-service started receiving\n";

    for (bool noThrow = false; !noThrow; ) {
        try {
            GetWeatherService(city);
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

    return WeatherDesc;
}


std::string NRequest::TRequestManager::GetWeather() {
    logger << TLevel::Debug << "the request_manager-service started receiving\n";

    for (bool noThrow = false; !noThrow; ) {
        try {
            GetWeatherService();
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

    return WeatherDesc;
}