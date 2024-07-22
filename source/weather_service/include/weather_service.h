#ifndef WEATHER_SERVICE_HEADER

#    include "boost/asio.hpp"
#    include "cache_manager.h"
#    include <memory>
#    include "nlohmann/json.hpp"
#    include "request_manager.h"

namespace NWeather {
    struct TQueryParams {
        std::string Method;
        std::string Resource;
        std::string Connection;
        std::string ContentLength;
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

        std::string FormAnswer(const std::string& query) const;

        void ServeTheClient(std::unique_ptr<TSocket> clientSock) const;
    
    };
}

#endif

#define WEATHER_SERVICE_HEADER
