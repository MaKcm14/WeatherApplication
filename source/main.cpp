#include <iostream>
#include "weather_service.h"

int main() {
    try {
        NWeather::TWeatherService::RunService();

    } catch (NRequest::TRequestException& excp) {
        std::cout << excp.what() << std::endl;

    } catch (NWeather::TWeatherException& excp) {
        std::cout << excp.what() << std::endl;
    
    } catch (std::exception& excp) {
        std::cout << excp.what() << std::endl;
        
    } catch (...) {
        std::cout << "error generated\n";
    }

    return 0;
}