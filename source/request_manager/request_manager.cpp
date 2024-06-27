#include "include/request_manager.h"

/// TODO: add some description to code for more information in logs
/// TODO: check all code matches the Yandex code style
/// TODO: check all code have maximum optimization

extern TLogger logger;

namespace NRequest {
    boost::asio::io_service service;
    boost::asio::ip::tcp::endpoint epRequest;
}


NRequest::TMRequest::TMRequest(const std::string& city)
    : SocketWeath(NRequest::service)
    , SocketGeo(NRequest::service)
    , City(city)
{
}


NRequest::TMRequest::TMRequest()
    : SocketWeath(NRequest::service)
    , SocketGeo(NRequest::service)
{
}


void NRequest::InitializeNetParams() {
    static bool isInit = false;
    if (isInit) {
        return;
    }

    try {
        boost::asio::ip::tcp::resolver resolver(service);
        boost::asio::ip::tcp::resolver::query query("api.openweathermap.org", "80");
        boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        
        logger << ELevel::Info << "got ip for domen api.openweather.org" << '\n';
        NRequest::epRequest = std::move(*iter);

        isInit = true;
        
    } catch (boost::system::system_error& excp) {
        //logger << ELevel::Error << "~ InitializeNetParams() error: " << excp.what() << '\n';
        throw TRException(excp.what());
    }

}


std::string NRequest::TMRequest::GetMmHg(std::string pressure) const {
    std::istringstream pressIn(pressure);
    std::ostringstream pressOut;
    double press;
    pressIn >> press;

    press /= 1.333;
    pressOut << press;

    return pressOut.str();
}


std::string NRequest::TMRequest::GetCelsus(std::string temp) const {
    std::istringstream tempIn(temp);
    std::ostringstream tempOut;

    double tempNum;
    tempIn >> tempNum;

    tempNum -= 273;
    tempOut << tempNum;

    return tempOut.str();
}


void NRequest::TMRequest::SetWeatherDesc(const nlohmann::json& weathJson) {
    WeatherDesc.clear();
    try {
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

    } catch (nlohmann::json::type_error& typeExcp) {
        //logger << "~ SetWeatherDesc() error: " << typeExcp.what() << '\n';
        //logger << "  error's id: " << typeExcp.id << '\n';
        throw TRException(typeExcp.what());

    } catch (nlohmann::json::parse_error& parseExcp) {
        //logger << "~ SetWeatherDesc() error: " << parseExcp.what() << '\n';
        //logger << "  error's id: " << parseExcp.id << '\n';
        throw TRException(parseExcp.what());

    }
}


std::string NRequest::TMRequest::GetCoordsJson(const std::string& city) {
    std::string coordsBuff(4096, '\0');
    try {
        std::string url = "http://api.openweathermap.org/geo/1.0/direct?q=";

        url += city;
        url += ",,RU&limit=1&"; 
        url += "&appid=55fd63b9fc0df56901b0ae195b65c16b";

        SocketGeo.connect(NRequest::epRequest);

        SocketGeo.write_some(boost::asio::buffer(
            "GET " + url + "\r\n"
            "Host: api.openweathermap.org\r\n"
            "APPID: 55fd63b9fc0df56901b0ae195b65c16b\r\n"
            "\r\n\r\n"));
        
        SocketGeo.read_some(boost::asio::buffer(coordsBuff));

    } catch (boost::system::system_error& excp) {
        //logger << "~ GetCoordsJson() error: " << excp.what() << '\n';
        throw TRException(excp.what());
    }

    return coordsBuff;
 }


std::unordered_map<std::string, std::string> NRequest::TMRequest::GetCoords(std::string jsonStrCoords) {
    std::unordered_map<std::string, std::string> coords = {{ "lat", "" }, { "lon", "" }};
    nlohmann::json jsonCoords;

    try {
        jsonCoords = nlohmann::json::parse(jsonStrCoords);
        coords.at("lat") = jsonCoords.at(0).at("lat").dump();
        coords.at("lon") = jsonCoords.at(0).at("lon").dump();

    } catch (nlohmann::json::parse_error& excp) {
        //logger << "~ GetCoords() error: " << excp.what() << '\n';
        //logger << "  error' id: " << excp.id << '\n';
        throw TRException(excp.what());

    } catch (nlohmann::json::type_error& typeExcp) {
        //logger << "~ GetCoords() error: " << typeExcp.what() << '\n';
        //logger << "  error's id: " << typeExcp.id << '\n';
        throw TRException(typeExcp.what());

    } catch (std::exception& excp) {
        //logger << "~ GetCoords() error: " << excp.what() << '\n';
        throw TRException(excp.what());

    }
    
    return coords;
}


std::string NRequest::TMRequest::GetWeatherJson(
    const std::unordered_map<std::string, std::string>& coords)
{
    /// TODO: change API Key and turn this one off

    std::string weathBuff(4096, '\0');
    try {
        std::string url = "http://api.openweathermap.org/data/2.5/weather?lat=";

        url += coords.at("lat");
        url += "&lon=";
        url += coords.at("lon");
        url += "&appid=55fd63b9fc0df56901b0ae195b65c16b";

        SocketWeath.connect(NRequest::epRequest);

        SocketWeath.write_some(boost::asio::buffer(
            "GET " + url + "\n\r"
            "Host: api.openweathermap.org\r\n"
            "APPID: 55fd63b9fc0df56901b0ae195b65c16b\r\n"
            "\n\r\n\r"));
        
        SocketWeath.read_some(boost::asio::buffer(weathBuff));

    } catch (boost::system::system_error& excp) {
        //logger << "~ GetWeatherJson() error: " << excp.what() << '\n';
        throw TRException(excp.what());
    } catch (std::exception& excp) {
        //logger << "~ GetWeatherJson() error: " << excp.what() << '\n';
        throw TRException(excp.what());
    } 

   return weathBuff;
}


std::string NRequest::TMRequest::GetWeather(const std::string& city) {
    if (City == city && !WeatherDesc.empty()) {
        return WeatherDesc;
    }

    City = city;
    try {
        nlohmann::json weathJson = nlohmann::json::parse(GetWeatherJson(GetCoords(GetCoordsJson(city))));
        
        SetWeatherDesc(weathJson);

    } catch (nlohmann::json::parse_error& excp) {
        City.clear();
        WeatherDesc.clear();
        //logger << "~ GetWeather() error: " << excp.what() << '\n';
        //logger << "  error's id: " << excp.id << '\n';
        throw TRException(excp.what());
    } catch (nlohmann::json::type_error& typeExcp) {
        City.clear();
        WeatherDesc.clear();
        //logger << "~ GetWeather() error: " << typeExcp.what() << '\n';
        //logger << "  error's id: " << typeExcp.id << '\n';
        throw TRException(typeExcp.what());
    }

    return WeatherDesc;
}


std::string NRequest::TMRequest::GetWeather() {
    if (!City.empty()) {
        return GetWeather(City);
    } else {
        throw std::invalid_argument("the member City is empty");
    }
}
