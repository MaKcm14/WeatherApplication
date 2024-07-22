#include <iostream>
#include "weather_service.h"

TLogger logger;

int main() {
    try {
        /*
        NRequest::ConfigureRequestService();    
        NRequest::TCacheManager cache;
        NRequest::TRequestManager request;

        std::string city;
        std::cin >> city;

        if (cache.IsDataExpired(city)) {
            auto weatherDesc = request.GetWeather(city);
            cache.InsertOrUpdateData(city, weatherDesc);
            std::cout << weatherDesc;
            
        } else {
            std::cout << cache.GetData(city);
        }*/

        NWeather::TWeatherService service;

        service.RunService();

    } catch (NRequest::TRequestException& excp) {
        std::cout << excp.what();
    } catch (...) {
        std::cout << "error generated\n";
    }

    return 0;
}