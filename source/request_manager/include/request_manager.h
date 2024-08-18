#ifndef REQUEST_MANAGER_HEADER

#    include <boost/asio.hpp>
#    include <chrono>
#    include <fstream>
#    include <iostream>
#    include "logger.h"
#    include <nlohmann/json.hpp>
#    include "request_exception.h"
#    include <sstream>
#    include <unordered_map>
#    include <vector>

namespace NRequest {

    using TTcpSocket = boost::asio::ip::tcp::socket;

    void InitConfig();

    void ConfigureRequestService();

    /// @brief makes converting for different objects
    class TConverter {
    public:
        std::string ConvertToUrlView(const std::string& obj) const;

        std::string ConvertPascalsToMmHg(std::string pressure) const;

        std::string ConvertKelvinsToCelsus(std::string temp) const;

        std::string ConvertWeatherJsonToWeatherTemplate(const nlohmann::json& weathJson);

    };


    /// @brief main weather module requests for weather
    class TWeatherModule {
    public:
        TWeatherModule();

        ~TWeatherModule() {
            SocketWeath.close();
        }

        
        std::string TryGetWeatherTemplate(const std::string& city);

        std::string GetCity() const noexcept {
            return City;
        }

        std::string GetWeatherDescription() const noexcept {
            return WeatherDesc;
        }
        
        static void InitWeatherApiKey();

        static void InitWeatherEndpoints();


    private:
        std::string GetWeatherJson();


    public:
        inline static boost::asio::io_service RequestService;
        inline static boost::asio::ip::tcp::endpoint EpOpenWeatherRequest;

    private:
        TConverter Converter;
        TTcpSocket SocketWeath;
        std::string City;
        std::string WeatherDesc;
        inline static std::string OpenWeatherApiKey = "";

    };
    

    /// @brief makes requests
    class TRequestManager {
    public:
        std::string GetWeatherTemplate(const std::string& city);
        
        
    private:
        TWeatherModule WeatherModule;
        
    };

}


#endif


#define REQUEST_MANAGER_HEADER
