#include "include/request_manager.h"

extern TLogger logger;


namespace NRequest {
    boost::asio::io_service service;
    boost::asio::ip::tcp::endpoint epRequest;
}


NRequest::TMRequest::TMRequest()
    : SocketWeath(NRequest::service)
    , SocketGeo(NRequest::service)
{
    InitApiKey();
}


NRequest::TMRequest::TMRequest(const std::string& city)
    : SocketWeath(NRequest::service)
    , SocketGeo(NRequest::service)
    , City(city)
{
    InitApiKey();
}


void NRequest::TMRequest::InitApiKey() {
    std::ifstream apiStream("../API_KEY.txt");

    if (!apiStream.is_open()) {
        logger << TLevel::Error << "file with API_KEY wasn't opened\n\n";
        throw TRException("can't open the API_KEY.txt");
    }

    apiStream >> NRequest::TMRequest::ApiKey;
}


void NRequest::InitializeNetParams() {
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
        logger << TLevel::Error << "~ InitializeNetParams() error: " << excp.what() << "\n\n";
        throw TRException(excp.what());
    }

}


std::string NRequest::TMRequest::GetMmHg(std::string pressure) const {
    double press;
    std::istringstream pressIn(std::move(pressure));
    std::ostringstream pressOut;

    pressIn >> press;
    press /= 1.333;
    pressOut << press;

    return pressOut.str();
}


std::string NRequest::TMRequest::GetCelsus(std::string temp) const {
    double tempNum;
    std::istringstream tempIn(std::move(temp));
    std::ostringstream tempOut;

    tempIn >> tempNum;
    tempNum -= 273.;
    tempOut << tempNum;

    return tempOut.str();
}


void NRequest::TMRequest::SetWeatherDesc(const nlohmann::json& weathJson) {
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
        WeatherDesc += weathJson.at("wind").at("gust").dump() + "\n";

        logger << TLevel::Debug << "the weather description was successfully made\n\n";

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Error << "~ SetWeatherDesc() error: " << typeExcp.what() << "\n";
        logger << TLevel::Error << "\terror's id: " << typeExcp.id << "\n\n";

        throw TRException(typeExcp.what());

    } catch (nlohmann::json::parse_error& parseExcp) {
        logger << TLevel::Error << "~ SetWeatherDesc() error: " << parseExcp.what() << "\n";
        logger << TLevel::Error << "  error's id: " << parseExcp.id << "\n\n";
        throw TRException(parseExcp.what());
    }
}


std::string NRequest::TMRequest::GetCoordsJson() {
    std::string coordsBuff(4096, '\0');

    try {
        logger << TLevel::Debug << "began sending the request to 'api.openweathermap.org' (geo-service) ";
        logger << "for receiving the coordinates of the city '" << City << "'\n";

        std::string url = "http://api.openweathermap.org/geo/1.0/direct?q=";
        std::ostringstream requestStream;

        url += City;
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

    } catch (boost::system::system_error& excp) {
        logger << TLevel::Error << "~ GetCoordsJson() error: " << excp.what() << "\n\n";
        throw TRException(excp.what());
    }

    return coordsBuff;
 }


auto NRequest::TMRequest::GetCoords(const std::string& jsonStrCoords) const {
    nlohmann::json jsonCoords;
    std::unordered_map<std::string, std::string> coords = {{ "lat", "" }, { "lon", "" }};

    try {
        logger << TLevel::Debug << "parsing the json from 'api.openweathermap.org' ";
        logger << "(geo-service) has begun\n";

        jsonCoords = std::move(nlohmann::json::parse(jsonStrCoords));

        coords.at("lat") = jsonCoords.at(0).at("lat").dump();
        coords.at("lon") = jsonCoords.at(0).at("lon").dump();

        logger << TLevel::Debug << "parsing the json from 'api.openweathermap.org' ";
        logger << "(geo-service) was successfully finished\n\n";

    } catch (nlohmann::json::parse_error& excp) {
        logger << TLevel::Error << "~ GetCoords() error: " << excp.what() << "\n";
        logger << TLevel::Error << "  error' id: " << excp.id << "\n\n";

        throw TRException(excp.what());

    } catch (nlohmann::json::type_error& typeExcp) {
        logger << TLevel::Error << "~ GetCoords() error: " << typeExcp.what() << "\n";
        logger << TLevel::Error << "  error's id: " << typeExcp.id << "\n\n";

        throw TRException(typeExcp.what());

    } catch (std::exception& excp) {
        logger << TLevel::Error << "~ GetCoords() error: " << excp.what() << "\n\n";
        throw TRException(excp.what());
    }
    
    return coords;
}


auto NRequest::TMRequest::GetWeatherJson(const std::unordered_map<std::string, std::string>& coords) {
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

    } catch (boost::system::system_error& excp) {
        logger << TLevel::Error << "~ GetWeatherJson() error: " << excp.what() << "\n\n";
        throw TRException(excp.what());

    } catch (std::exception& excp) {
        logger << TLevel::Error << "~ GetWeatherJson() error: " << excp.what() << "\n\n";
        throw TRException(excp.what());
    } 

   return weathBuff;
}


std::string NRequest::TMRequest::GetWeather(std::string city) {
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

    } catch (nlohmann::json::parse_error& excp) {
        City.clear();
        WeatherDesc.clear();
        logger << TLevel::Error << "~ GetWeather() error: " << excp.what() << "\n";
        logger << TLevel::Error << "  error's id: " << excp.id << "\n\n";

        throw TRException(excp.what());

    } catch (nlohmann::json::type_error& typeExcp) {
        City.clear();
        WeatherDesc.clear();
        logger << TLevel::Error << "~ GetWeather() error: " << typeExcp.what() << "\n";
        logger << TLevel::Error << "  error's id: " << typeExcp.id << "\n\n";

        throw TRException(typeExcp.what());
    }

    logger << TLevel::Debug << "the weather description was successfully made:\n";
    logger << TLevel::Debug << WeatherDesc << "\n";

    return WeatherDesc;
}


std::string NRequest::TMRequest::GetWeather() {
    if (!City.empty()) {
        return GetWeather(City);
    } else {
        logger << TLevel::Error << "got invalid argument for city\n";
        throw std::invalid_argument("the member City is empty");
    }
}
