#ifndef REQUEST_MANAGER_HEADER

#    include <boost/asio.hpp>
#    include <chrono>
#    include <fstream>
#    include <iostream>
#    include "logger.h"
#    include <mutex>
#    include <nlohmann/json.hpp>
#    include "request_exception.h"
#    include <sstream>
#    include <thread>
#    include <unordered_map>
#    include <vector>

namespace NRequest {

    void InitNetParams();

    void InitConfig();

    void ConfigureRequestService();

    /*
    struct TThreadSettings {
        bool CriticalErrorFlag = false;
        bool WeatherInitFlag = false;
        std::mutex WeatherMut;
    };

    void GetWeatherThreadService(const std::string& city, std::string& weather, TThreadSettings& setts);

    void MakeThreads(std::vector<std::thread>& requestWeathThreads, const std::string& city, std::string& weatherDescResult, TThreadSettings& setts);

    std::string GetWeather(const std::string& city);
    */

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

        std::string GetWeather(const std::string& city);

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

        static void InitApiKey();

    private:
        
        std::string GetCoordsJson();

        auto GetCoords(const std::string& jsonStrCoords) const;

        auto GetWeatherJson(const std::unordered_map<std::string, std::string>& coords);

        std::string GetMmHg(std::string pressure) const;

        std::string GetCelsus(std::string temp) const;

        void SetWeatherDesc(const nlohmann::json& weathJson);

        std::string GetRightCityName(const std::string& city) const noexcept;

        std::string GetWeatherService();

        std::string GetWeatherService(const std::string& city);

    private:
        static inline std::string ApiKey = "";
        TTcpSocket SocketWeath;
        TTcpSocket SocketGeo;
        std::string City;
        std::string WeatherDesc;
        
    };

}


#endif

#define REQUEST_MANAGER_HEADER
