#include "request_manager.h"
#include "logger.h"

TLogger logger;

int main() {
    logger << ELevel::Info << "start server\n";
    NRequest::TMRequest testRequest("Moscow");

    NRequest::InitializeNetParams();

    std::cout << testRequest.GetWeather();

    return 0;
}