#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TLogger logger;

int main() {
    try {
        NRequest::TMRequest testRequest("Moscow");
        NRequest::InitializeNetParams();

        std::cout << testRequest.GetWeather();

    } catch (...) {
        return 1;
    }

    return 0;
}