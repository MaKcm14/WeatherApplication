#include "include/request_manager.h"

extern TLogger logger;

namespace NRequest {
    boost::asio::io_service service;
    boost::asio::ip::tcp::endpoint epRequest;
}


void NRequest::InitConfig() {
    std::ifstream configStream("../configuration.json");
    
    logger << TLevel::Info << "openning the file with configuration\n";

    if (!configStream.is_open()) {
        logger << TLevel::Fatal << "file with configuration of the programme wasn't opened\n\n";
        throw TRequestException("can't open the configuration.json", 402);
    }

    try {
        logger << TLevel::Info << "deserealization of the configuration has begun\n";
        configStream >> configJson;

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Fatal << "error while the deserealization of the configuration.json:\n\t";
        logger << parseExcp.what() << "\n\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);
    }

    logger << TLevel::Info << "deserealization of the configuration finished successfully\n\n";
}


static void NRequest::InitNetParams() {
    static bool isInit = false;
    if (isInit) {
        return;
    }

    try {
        logger << TLevel::Info << "began receiving the ip from resolver for 'api.openweathermap.org'\n";

        boost::asio::ip::tcp::resolver resolver(service);
        boost::asio::ip::tcp::resolver::query query("api.openweathermap.org", "80");
        boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        
        logger << TLevel::Info << "got ip for domen 'api.openweathermap.org' from resolver\n";
       
        NRequest::epRequest = std::move(*iter);
        isInit = true;
        
    } catch (boost::system::system_error& excp) {
        logger << TLevel::Fatal << "~ InitializeNetParams() error: " << excp.what() << "\n\n";
        throw TRequestException(excp.what(), 501);
    }

}


NRequest::TRequestManager::TRequestManager()
    : SocketWeath(NRequest::service)
    , SocketGeo(NRequest::service)
{
    InitConfig();
    InitNetParams();

    try {
        logger << TLevel::Info << "initializing the ApiKey for the 'api.openweathermap.org\n";
        ApiKey = configJson.at("API_KEY").dump();

        logger << TLevel::Info << "initializing the ApiKey for the 'api.openweathermap.org finished ";
        logger << "correctly\n\n";

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Fatal << "error while initializing the ApiKey: " << typeExcp.what() << "\n";
        logger << TLevel::Fatal << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Fatal << "error while initializing the ApiKey: " << parseExcp.what() << "\n";
        logger << TLevel::Fatal << "\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);
    }
}


NRequest::TRequestManager::TRequestManager(const std::string& city)
    : SocketWeath(NRequest::service)
    , SocketGeo(NRequest::service)
    , City(city)
{
    InitConfig();
    InitNetParams();

    try {
        logger << TLevel::Info << "initializing the ApiKey for the 'api.openweathermap.org\n";
        ApiKey = configJson.at("API_KEY").dump();

        logger << TLevel::Info << "initializing the ApiKey for the 'api.openweathermap.org finished ";
        logger << "correctly\n\n";

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Fatal << "error while initializing the ApiKey: " << typeExcp.what() << "\n";
        logger << TLevel::Fatal << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Fatal << "error while initializing the ApiKey: " << parseExcp.what() << "\n";
        logger << TLevel::Fatal << "\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);
    }
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
    WeatherDesc.clear();

    try {
        logger << TLevel::Debug << "the creation of the weather description has begun\n";

        WeatherDesc += "- Weather Main Description: ";
        WeatherDesc += weathJson.at("weather").at(0).at("description").dump() + "\n";

        WeatherDesc += "- Temperature (C): ";
        WeatherDesc += GetCelsus(weathJson.at("main").at("temp").dump()) + "\n";

        WeatherDesc += "- Temperature 'feels_like' (C): ";
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

        logger << TLevel::Debug << "the weather description was successfully made\n\n";

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Error << "~ SetWeatherDesc() error: " << typeExcp.what() << "\n";
        logger << TLevel::Error << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Error << "~ SetWeatherDesc() error: " << parseExcp.what() << "\n";
        logger << TLevel::Error << "\terror's id: " << parseExcp.id << "\n\n";
        throw TRequestException(parseExcp.what(), 502);
    }
}


std::string NRequest::TRequestManager::GetCoordsJson() {
    std::string coordsBuff(4096, '\0');

    try {
        logger << TLevel::Debug << "began sending the request to 'api.openweathermap.org' (geo-service) ";
        logger << "for receiving the coordinates of the city '" << City << "'\n";

        std::string url = "http://api.openweathermap.org/geo/1.0/direct?q=";
        std::ostringstream requestStream;

        url += GetRightCityName(City);
        url += ",,RU&limit=1&"; 
        url += "&appid=";
        url += ApiKey;

        SocketGeo.connect(NRequest::epRequest);

        requestStream << "GET " << url << "\r\n";
        requestStream << "Host: api.openweathermap.org\r\n";
        requestStream << "APPID: " << ApiKey << "\r\n";
        requestStream << "\r\n\r\n";

        logger << TLevel::Debug << "sent the HTTP-message:\n" << requestStream.str() << "\n"; 
        
        SocketGeo.write_some(boost::asio::buffer(requestStream.str()));
        SocketGeo.read_some(boost::asio::buffer(coordsBuff));

        logger << TLevel::Debug << "the coordinates was got from ";
        logger << "'api.openweathermap.org' (geo-service):\n";
        logger << coordsBuff.substr(0, coordsBuff.find('\0')) << "\n\n";

        SocketGeo.close();

        if ((coordsBuff.substr(0, coordsBuff.find('\0'))).size() == 2) {
            logger << TLevel::Error << "the coordinates from 'api.openweathermap.org' ";
            logger << "(geo-service) are empty\n\n";
            throw TRequestException("client-side error: json is empty", 401);
        }

    } catch (boost::system::system_error& excp) {
        SocketGeo.close();
        logger << TLevel::Fatal << "~ GetCoordsJson() error: " << excp.what() << "\n\n";
        throw TRequestException(excp.what(), 501);

    } catch (...) {
        SocketGeo.close();
        throw;
    }

    return coordsBuff;
 }


auto NRequest::TRequestManager::GetCoords(const std::string& jsonStrCoords) const {
    nlohmann::json jsonCoords;
    std::unordered_map<std::string, std::string> coords = {{ "lat", "" }, { "lon", "" }};

    try {
        logger << TLevel::Debug << "parsing the json from 'api.openweathermap.org' ";
        logger << "(geo-service) has begun\n";

        jsonCoords = std::move(nlohmann::json::parse(jsonStrCoords));

        if (jsonCoords.at(0).at("name").dump() != ("\"" + City + "\"")) {
            logger << TLevel::Error << "the json with coordinates doesn't describe the city '";
            logger << City << "'\n\n";
            throw TRequestException("the invalid name of the city", 401);
        }

        coords.at("lat") = jsonCoords.at(0).at("lat").dump();
        coords.at("lon") = jsonCoords.at(0).at("lon").dump();

        logger << TLevel::Debug << "parsing the json from 'api.openweathermap.org' ";
        logger << "(geo-service) was successfully finished\n\n";

    } catch (nlohmann::json::parse_error& excp) {
        logger << TLevel::Error << "~ GetCoords() error: " << excp.what() << "\n";
        logger << TLevel::Error << "\terror' id: " << excp.id << "\n\n";
        throw TRequestException(excp.what(), 502);

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Error << "~ GetCoords() error: " << typeExcp.what() << "\n";
        logger << TLevel::Error << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);

    } catch (TRequestException& excp) {
        throw;

    } catch (std::exception& excp) {
        logger << TLevel::Error << "~ GetCoords() error: " << excp.what() << "\n\n";
        throw TRequestException(excp.what(), 503);
    }
    
    return coords;
}


auto NRequest::TRequestManager::GetWeatherJson(const std::unordered_map<std::string, std::string>& coords) {
    std::string weathBuff(4096, '\0');

    try {
        logger << TLevel::Debug << "began sending the request to 'api.openweathermap.org' ";
        logger << "(weather-service) for receiving the weather of the city '" << City << "'\n";

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

        logger << TLevel::Debug << "sent the HTTP-message:\n" << requestStream.str() << "\n"; 

        SocketWeath.write_some(boost::asio::buffer(requestStream.str()));
        SocketWeath.read_some(boost::asio::buffer(weathBuff));

        logger << TLevel::Debug << "the weather was got from 'api.openweathermap.org' ";
        logger << "(weather-service)\n";
        logger << TLevel::Debug << weathBuff.substr(0, weathBuff.find('\0')) << "\n\n";

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
        throw;
    }

   return weathBuff;
}


std::string NRequest::TRequestManager::GetWeather(std::string city) {
    if (City == city && !WeatherDesc.empty()) {
        logger << TLevel::Debug << "got the same city: return previous result\n";
        return WeatherDesc;
    }

    City = std::move(city);

    try {
        logger << TLevel::Debug << "the process of receiving the weather description ";
        logger << "(request_manager-service) for the city '" << City << "' has begun\n";

        nlohmann::json weathJson = nlohmann::json::parse(GetWeatherJson(GetCoords(GetCoordsJson())));
        SetWeatherDesc(weathJson);

        logger << TLevel::Debug << "the weather description was successfully made:\n";
        logger << WeatherDesc << "\n\n";

    } catch (nlohmann::json::parse_error& excp) {
        WeatherDesc.clear();
        logger << TLevel::Error << "~ GetWeather() error: " << excp.what() << "\n";
        logger << TLevel::Error << "\terror's id: " << excp.id << "\n\n";
        throw TRequestException(excp.what(), 502);

    } catch (nlohmann::json::type_error& typeExcp) {
        WeatherDesc.clear();
        logger << TLevel::Error << "~ GetWeather() error: " << typeExcp.what() << "\n";
        logger << TLevel::Error << "\terror's id: " << typeExcp.id << "\n\n";
        throw TRequestException(typeExcp.what(), 502);
    }

    return WeatherDesc;
}


std::string NRequest::TRequestManager::GetWeather() {
    if (!City.empty()) {
        return GetWeather(City);
    } else {
        logger << TLevel::Error << "got invalid argument for city\n";
        throw TRequestException("the member City is empty", 401);
    }
}
