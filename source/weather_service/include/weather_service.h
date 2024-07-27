#ifndef WEATHER_SERVICE_HEADER

#    include <atomic>
#    include "boost/asio.hpp"
#    include "cache_manager.h"
#    include <memory>
#    include "nlohmann/json.hpp"
#    include "request_manager.h"
#    include <thread>
#    include <unordered_map>
#    include "weather_exception.h"

namespace NWeather {

    struct TQueryParams {
        std::string Method;
        std::string Resource;
        std::string Data;
    };

    class TWeatherService {
    public:
        using TSocket = boost::asio::ip::tcp::socket; 

        static void RunService();

    private:
        static void ThreadServeClient(std::unique_ptr<TSocket> clientSock);

        TQueryParams ParseQuery(const std::string& query) const;

        std::string GetQueryHandler(const std::string& resource) const;

        std::string PostQueryHandler(const std::string& resource, const std::string& data) const;

        std::string FormResponse(const std::string& query) const;

        void ServeTheClient(std::unique_ptr<TSocket> clientSock) const;

        std::string ConvertWeatherToHtml(const std::string& weather) const;

    private:
        const inline static std::unordered_map<std::string, std::string> ErrorCodesAndResponses = {
            { "400", "HTTP/1.1 400 Bad Request\r\nCache-Control: no-store\r\nServer: localhost\r\n" \
                    "Connection: Close\r\nContent-Type: text/html; charset=ascii\r\n" },
            
            { "404", "HTTP/1.1 404 Not Found\r\nCache-Control: no-store\r\nServer: localhost\r\n" \
                    "Connection: Close\r\nContent-Type: text/html; charset=ascii\r\n" },

            { "405", "HTTP/1.1 405 Method Not Allowed\r\nCache-Control: no-store\r\nServer: localhost\r\n" \
                    "Connection: Close\r\nContent-Type: text/html; charset=ascii\r\n" },
            
            { "500", "HTTP/1.1 500 Internal Server Error\r\nCache-Control: no-store\r\nServer: localhost\r\n" \
                    "Connection: Close\r\nContent-Type: text/html; charset=ascii\r\n" },

            { "502", "HTTP/1.1 502 Bad Gateway\r\nCache-Control: no-store\r\nServer: localhost\r\n" \
                    "Connection: Close\r\nContent-Type: text/html; charset=ascii\r\n" }            
        };

        const inline static std::unordered_map<std::string, std::string> ResourcesToFiles = {
            { "/find", "/find.html" },
            { "/clouds", "/clouds.jpg" }
        };

    };
}

#endif

#define WEATHER_SERVICE_HEADER
