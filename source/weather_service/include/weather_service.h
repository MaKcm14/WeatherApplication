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

    /// @brief converts the view of information
    class TQueryConverter {
    public:
        std::string ConvertWeatherToHtml(const std::string& weather) const;

    };


    /// @brief handles the queries from the clients for the weather service
    class TQueryHandler {
    public:
        /// @brief keeps main query parameters
        struct TQueryParams {
            std::string Method;
            std::string Resource;
            std::string Data;

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


        TQueryParams ParseQuery(const std::string& query) const;

        std::string FormResponse(const std::string& query) const;

    };


    /// @brief  main class for other methods
    class TMethodHandler {
    public:
        virtual std::string IProcessRequest(const TQueryHandler::TQueryParams& query) const = 0;

    };


    /// @brief main class for GET method handling
    class TGetMethod : public TMethodHandler {
    public:
        std::string IProcessRequest(const TQueryHandler::TQueryParams& query) const override;

    };


    /// @brief main cass for POST method handling
    class TPostMethod : public TMethodHandler {
    public:
        std::string IProcessRequest(const TQueryHandler::TQueryParams& query) const override;


    private:
        std::string WeatherResourceHandler(const TQueryHandler::TQueryParams& query) const;

    
    private:
        TQueryConverter Converter;

    };


    /// @brief serves the clients (issues the weather description)
    class TWeatherService {
    public:
        using TTcpSocket = boost::asio::ip::tcp::socket; 

        static void RunService();


    private:
        static void ThreadServeClient(std::unique_ptr<TTcpSocket> clientSock);

        void ServeTheClient(std::unique_ptr<TTcpSocket> clientSock) const;


    private:
        TQueryHandler QueryHandler;

    };

}


#endif


#define WEATHER_SERVICE_HEADER
