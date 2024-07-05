#ifndef REQUEST_MANAGER_HEADER

#    include <boost/asio.hpp>
#    include <fstream>
#    include <iostream>
#    include "logger.h"
#    include <nlohmann/json.hpp>
#    include "request_exception.h"
#    include <sstream>
#    include <unordered_map>

namespace NRequest {

    nlohmann::json configJson;

    static void InitNetParams();

    void InitConfig();

    /// @brief Let make requests for 'api.openweathermap.org'
    class TRequestManager {
    public:
        using TTcpSocket = boost::asio::ip::tcp::socket;

        TRequestManager();

        TRequestManager(const std::string& city);

        ~TRequestManager() {
            SocketGeo.close();
            SocketWeath.close();
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
        std::string GetCoordsJson();

        auto GetCoords(const std::string& jsonStrCoords) const;

        auto GetWeatherJson(const std::unordered_map<std::string, std::string>& coords);

        std::string GetMmHg(std::string pressure) const;

        std::string GetCelsus(std::string temp) const;

        void SetWeatherDesc(const nlohmann::json& weathJson);

        std::string GetRightCityName(const std::string& city) const noexcept;

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
