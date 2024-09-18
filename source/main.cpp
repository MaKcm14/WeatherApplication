// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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