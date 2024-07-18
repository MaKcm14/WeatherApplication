#include "include/request_manager.h"

extern TLogger logger;

namespace NRequest {
    //std::mutex loggerMut;
    nlohmann::json configJson;
    boost::asio::io_service service;
    boost::asio::ip::tcp::endpoint epRequest;
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
        boost::asio::ip::tcp::resolver resolver(service);
        boost::asio::ip::tcp::resolver::query query("api.openweathermap.org", "80");
        boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        
        logger << TLevel::Info << "got ip for domen 'api.openweathermap.org' from resolver\n";
       
        NRequest::epRequest = std::move(*iter);

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
    : SocketWeath(NRequest::service)
    , SocketGeo(NRequest::service)
{
}


NRequest::TRequestManager::TRequestManager(const std::string& city)
    : SocketWeath(NRequest::service)
    , SocketGeo(NRequest::service)
    , City(city)
{
}


std::string NRequest::TRequestManager::GetRightCityName(const std::string& city) const noexcept {
    static const std::unordered_map<std::string, std::string> citiesNames = 
    { { "Saint Petersburg", "Saint-Petersburg" } }; 
    
    if (auto itFind = citiesNames.find(city); itFind != citiesNames.end()) {
        return itFind->second;
    } else {
        return city;
    }
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


void NRequest::TRequestManager::SetWeatherDesc(const nlohmann::json& weathJson) {
    logger << TLevel::Debug << "the creation of the weather description has begun\n";

    WeatherDesc.clear();

    try {
        WeatherDesc += "- Weather Main Description: ";
        WeatherDesc += weathJson.at("weather").at(0).at("description").dump() + "\n";

        WeatherDesc += "- Temperature (C): ";
        WeatherDesc += GetCelsus(weathJson.at("main").at("temp").dump()) + "\n";

        WeatherDesc += "- Temperature \"feels_like\" (C): ";
        WeatherDesc += GetCelsus(weathJson.at("main").at("feels_like").dump()) + "\n";

        WeatherDesc += "- Pressure (mmHg): ";
        WeatherDesc += GetMmHg(weathJson.at("main").at("pressure").dump()) + "\n";

        WeatherDesc += "- Wind (speed/gust m/s): ";
        WeatherDesc += weathJson.at("wind").at("speed").dump() + "/";

        if (weathJson.at("wind").contains("gust")) {
            WeatherDesc += weathJson.at("wind").at("gust").dump() + "\n";
        } else {
            WeatherDesc += "0\n";
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


std::string NRequest::TRequestManager::GetCoordsJson() {
    std::string coordsBuff(4096, '\0');

    logger << TLevel::Debug << "began sending the request to 'api.openweathermap.org' (geo-service) ";
    logger << "for receiving the coordinates of the city '" << City << "'\n";

    try {
        std::string url = "http://api.openweathermap.org/geo/1.0/direct?q=";
        std::ostringstream requestStream;

        url += GetRightCityName(City);
        url += ",,RU&limit=1"; 
        url += "&appid=";
        url += ApiKey;

        SocketGeo.connect(NRequest::epRequest);

        requestStream << "GET " << url << "\r\n";
        requestStream << "Host: api.openweathermap.org\r\n";
        requestStream << "APPID: " << ApiKey << "\r\n";
        requestStream << "\r\n\r\n";

        SocketGeo.write_some(boost::asio::buffer(requestStream.str()));
        SocketGeo.read_some(boost::asio::buffer(coordsBuff));

        SocketGeo.close();

    } catch (boost::system::system_error& excp) {
        SocketGeo.close();
        logger << TLevel::Error << "~ GetCoordsJson() error: " << excp.what() << "\n\n";
        throw TRequestException(excp.what(), 501);

    } catch (...) {
        SocketGeo.close();
        logger << TLevel::Error << "~ GetCoordsJson() error: unpredictable error was generated\n\n";
        throw;
    }

    if ((coordsBuff.substr(0, coordsBuff.find('\0'))).size() == 2) {
        logger << TLevel::Error << "~ GetCoordsJson() error: ";
        logger << "the coordinates from 'api.openweathermap.org' ";
        logger << "(geo-service) are empty\n\n";
        throw TRequestException("coords json is empty: perhaps the city name IS NOT correct", 401);
    }

    logger << TLevel::Debug << "the coordinates of the city '" << City << "' were correctly recieved\n\n";

    return coordsBuff;
 }


auto NRequest::TRequestManager::GetCoords(const std::string& jsonStrCoords) const {
    nlohmann::json jsonCoords;
    std::unordered_map<std::string, std::string> coords = {{ "lat", "" }, { "lon", "" }};

    logger << TLevel::Debug << "parsing the json from 'api.openweathermap.org' ";
    logger << "(geo-service) has begun\n";

    try {
        jsonCoords = std::move(nlohmann::json::parse(jsonStrCoords));

        if (jsonCoords.at(0).at("name").dump() != ("\"" + City + "\"")) {
            logger << TLevel::Error << "the json with coordinates doesn't describe the city '";
            logger << City << "': perhaps the name of the city isn't correct\n\n";
            throw TRequestException("the invalid name of the city", 401);
        }

        coords.at("lat") = jsonCoords.at(0).at("lat").dump();
        coords.at("lon") = jsonCoords.at(0).at("lon").dump();

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Error << "~ GetCoords() error: " << parseExcp.what() << "\n";
        logger << TLevel::Error << "\terror' id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Error << "~ GetCoords() error: " << typeExcp.what() << "\n";
        logger << TLevel::Error << "\terror' id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (NRequest::TRequestException& excp) {
        throw;
        
    } catch (std::exception& excp) {
        logger << TLevel::Error << "~ GetCoords() error: " << excp.what() << "\n";
        throw TRequestException(excp.what(), 503);
    }

    logger << TLevel::Debug << "parsing the json from 'api.openweathermap.org' ";
    logger << "(geo-service) was successfully finished\n\n";

    return coords;
}


auto NRequest::TRequestManager::GetWeatherJson(const std::unordered_map<std::string, std::string>& coords) {
    std::string weathBuff(4096, '\0');

    logger << TLevel::Debug << "began sending the request to 'api.openweathermap.org' ";
    logger << "(weather-service) for receiving the weather of the city '" << City << "'\n";

    try {
        std::string url = "http://api.openweathermap.org/data/2.5/weather?lat=";
        std::ostringstream requestStream;

        url += coords.at("lat");
        url += "&lon=";
        url += coords.at("lon");
        url += "&appid=";
        url += ApiKey;

        SocketWeath.connect(NRequest::epRequest);

        requestStream << "GET " << url << "\r\n";
        requestStream << "Host: api.openweathermap.org\r\n";
        requestStream << "APPID: " << ApiKey << "\r\n";
        requestStream << "\r\n\r\n";

        SocketWeath.write_some(boost::asio::buffer(requestStream.str()));
        SocketWeath.read_some(boost::asio::buffer(weathBuff));

        if (weathBuff.empty()) {
            throw TRequestException("the weather json is empty", 401);
        }

        SocketWeath.close();

    } catch (boost::system::system_error& excp) {
        SocketWeath.close();
        logger << TLevel::Error << "~ GetWeatherJson() error: " << excp.what() << "\n\n";
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
    City = city;
    WeatherDesc.clear();

    logger << TLevel::Debug << "the process of receiving the weather description ";
    logger << "(request_manager-service) for the city '" << City << "' has begun\n";
    
    try {
        nlohmann::json weathJson = nlohmann::json::parse(GetWeatherJson(GetCoords(GetCoordsJson())));

        if (weathJson.at("cod").dump() != "200") {
            logger << TLevel::Error << "~ GetWeatherService() error: ";
            logger << "the answer from the server IS NOT 200 OK\n";
            throw TRequestException("the code status for the weather description isn't 200 (not OK)",
                505);
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
        return GetWeather(City);

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
            if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499) {
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
            if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499) {
                throw;
            }

        } catch (...) {
            throw;
        }
    }

    logger << TLevel::Debug << "the request_manager-service stopped receiving\n";

    return WeatherDesc;
}


/*
void NRequest::GetWeatherThreadService(const std::string& city, std::string& weather, TThreadSettings& setts) {
    NRequest::TRequestManager request(city);

    for (bool noThrow = false; !noThrow; ) {
        try {
            auto weathRes = std::move(request.GetWeather());

            setts.WeatherMut.lock();

            if (setts.WeatherInitFlag) {
                setts.WeatherMut.unlock();
                return;
            }

            weather = std::move(weathRes);
            noThrow = true;

        } catch (NRequest::TRequestException& excp) {
            if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499) {
                loggerMut.lock();

                logger << TLevel::Fatal << "get fatal error with code " << excp.GetErrorId() << "\n";
                logger << "\tdescription: " << excp.what() << "\n\n";

                loggerMut.unlock();

                setts.CriticalErrorFlag = true;

                break;
            }

            loggerMut.lock();
            logger << TLevel::Error << "get error: " << excp.what() << "\ncode: " << excp.what() << "\n\n";
            loggerMut.unlock();

        } catch (...) {
            loggerMut.lock();
            logger << TLevel::Error << "get unpredictable error\n\n";
            loggerMut.unlock();

        }
        setts.WeatherMut.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


void NRequest::MakeThreads(std::vector<std::thread>& requestWeathThreads, const std::string& city, std::string& weather, TThreadSettings& setts) {
    for (size_t i = 0; i != 4; ++i) {
        requestWeathThreads.push_back(std::thread(GetWeatherThreadService, std::cref(city), 
            std::ref(weather), std::ref(setts)));
        requestWeathThreads.back().detach();
    }
} 


std::string NRequest::GetWeather(const std::string& city) {
    logger << TLevel::Debug << "receiving the weather for the city '" << city << "' begun\n";

    std::string weatherDescResult;
    std::vector<std::thread> requestWeathThreads;
    TThreadSettings setts;

    MakeThreads(requestWeathThreads, city, weatherDescResult, setts);
    
    for (size_t count = 1; true; ++count) {
        setts.WeatherMut.lock();

        if (setts.CriticalErrorFlag) {
            setts.WeatherMut.unlock();
            
            loggerMut.lock();
            logger << TLevel::Fatal << "critical error was generated\n\n";
            loggerMut.unlock();

            throw TRequestException("critical error was generated\n", 600);
        }

        if (!weatherDescResult.empty()) {
            setts.WeatherInitFlag = true;
            setts.WeatherMut.unlock();
            break;

        } else if (count == 100'000'000) {
            MakeThreads(requestWeathThreads, city, weatherDescResult, setts);
            count = 1;
        }
        setts.WeatherMut.unlock();
    }

    loggerMut.lock();
    logger << TLevel::Debug << "receiving the weather for the city '" << city;
    logger << "' finished successfully:\n" << weatherDescResult << "\n\n";
    loggerMut.unlock();

    return weatherDescResult;
}
*/