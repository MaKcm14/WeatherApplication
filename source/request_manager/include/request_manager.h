#ifndef REQUEST_MANAGER_HEADER

#    include <boost/asio.hpp>
#    include <iostream>
#    include "logger.h"
#    include <nlohmann/json.hpp>
#    include "request_exception.h"
#    include <sstream>
#    include <unordered_map>

namespace NRequest {

    void InitializeNetParams();

    /// @brief Let make requests for 'api.openweathermap.org'
    class TMRequest {
    public:
        using TTcpSocket = boost::asio::ip::tcp::socket;

        TMRequest();

        TMRequest(const std::string& city);

        ~TMRequest() {
            SocketWeath.close();
            SocketGeo.close();
        }

        std::string GetWeather();

        std::string GetWeather(std::string city);

        std::string GetCity() const noexcept {
            return City;
        }

        void SetCity(const std::string& city) noexcept {
            City = city;
            WeatherDesc.clear();
        }

        bool IsWeathDescInit() const noexcept {
            return !WeatherDesc.empty();
        }

    private:
        void InitApiKey();
        
        std::string GetCoordsJson();

        auto GetCoords(const std::string& jsonStrCoords) const;

        auto GetWeatherJson(const std::unordered_map<std::string, std::string>& coords);

        std::string GetMmHg(std::string pressure) const;

        std::string GetCelsus(std::string temp) const;

        void SetWeatherDesc(const nlohmann::json& weathJson);

    private:
        std::string ApiKey;
        TTcpSocket SocketWeath;
        TTcpSocket SocketGeo;
        std::string City;
        std::string WeatherDesc;

    };

}


#endif

#define REQUEST_MANAGER_HEADER
