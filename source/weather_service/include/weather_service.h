#ifndef WEATHER_SERVICE_HEADER

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

        TWeatherService() {
            NRequest::ConfigureRequestService();
        }

        void RunService() const;

    private:
        TQueryParams ParseQuery(const std::string& query) const;

        std::string GetQueryHandler(const std::string& resource) const;

        std::string PostQueryHandler(const std::string& resource, const std::string& data) const;

        std::string FormResponse(const std::string& query) const;

        void ServeTheClient(std::unique_ptr<TSocket> clientSock) const;

        std::string ConvertWeatherToHtml(const std::string& weather) const;

    private:
        const inline static std::unordered_map<std::string, std::string> ErrorCodesAndResponses = {
            { "404", "HTTP/1.1 404 Not Found\r\nCache-Control: no-store\r\nServer: 127.0.0.1:8080\r\n" \
                    "Content-Length: 145\r\nConnection: Closed\r\nContent-Type: text/html; charset=ascii\r\n\r\n" },

            { "405", "HTTP/1.1 405 Not Acceptable\r\nCache-Control: no-store\r\nServer: 127.0.0.1:8080\r\n" \
                    "Content-Length: 161\r\nConnection: Closed\r\nContent-Type: text/html; charset=ascii\r\n\r\n" },

            { "406", "HTTP/1.1 406 Not Acceptable\r\nCache-Control: no-store\r\nServer: 127.0.0.1:8080\r\n" \
                    "Content-Length: 183\r\nConnection: Closed\r\nContent-Type: text/html; charset=ascii\r\n\r\n" },

            { "502", "HTTP/1.1 502 Bad Gateway\r\nCache-Control: no-store\r\nServer: 127.0.0.1:8080\r\n" \
                    "Content-Length: 152\r\nConnection: Closed\r\nContent-Type: text/html; charset=ascii\r\n\r\n" }            
        };

        const inline static std::unordered_map<std::string, std::string> ResourcesToFiles = {
            { "/find", "/find.html" },
            { "/clouds", "/clouds.jpg" }
        };
    
    };
}

#endif

#define WEATHER_SERVICE_HEADER
