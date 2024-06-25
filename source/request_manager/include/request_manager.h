#ifndef REQUEST_MANAGER_HEADER

#    include <boost/asio.hpp>
#    include <iostream>
#    include <nlohmann/json.hpp>
#    include <sstream>
#    include <unordered_map>

namespace NRequest {

    boost::asio::io_service service;
    boost::asio::ip::tcp::endpoint epRequest;

    void InitializeNetParams();

    class TMRequest {
    private:
        std::string GetCoordsJson(const std::string& city);

        std::string GetWeatherJson(const std::unordered_map<std::string, std::string>& coords);

        std::unordered_map<std::string, std::string> GetCoords(std::string jsonStrCoords);

        void SetWeatherDesc(const nlohmann::json& weathJson);

        std::string GetMmHg(std::string pressure) const;

        std::string GetCelsus(std::string temp) const;

    public:
        TMRequest(const std::string& city)
        : SocketWeath(NRequest::service)
        , SocketGeo(NRequest::service)
        , City(city)
        {
        }

        TMRequest() 
        : SocketWeath(NRequest::service)
        , SocketGeo(NRequest::service)
        {
        }

        ~TMRequest() {
            SocketWeath.close();
            SocketGeo.close();
        }

        std::string GetWeather(const std::string& city);

        std::string GetWeather();

        std::string GetCity() const noexcept {
            return City;
        }

        void SetCity(const std::string& city) noexcept {
            City = city;
            WeatherDesc.clear();
        } 

    private:
        boost::asio::ip::tcp::socket SocketWeath;
        boost::asio::ip::tcp::socket SocketGeo;
        std::string City;
        std::string WeatherDesc;
    };

}

#endif

#define REQUEST_MANAGER_HEADER
