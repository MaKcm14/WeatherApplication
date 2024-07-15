#include <iostream>
#include "logger.h"
#include "request_manager.h"

TLogger logger;

int main() {
    try {
        NRequest::ConfigureRequestService();
        NRequest::TRequestManager request;
        std::string city;

        std::cin >> city;
        std::cout << request.GetWeather(city);

    } catch (NRequest::TRequestException& excp) {
        std::cout << excp.what();
    } catch (...) {
        std::cout << "error generated\n";
    }

    return 0;
}