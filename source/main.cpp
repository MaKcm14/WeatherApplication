#include "logger.h"
#include "request_manager.h"

TLogger logger;

int main() {
    try {
        NRequest::TMRequest testRequest("Moscow");
        NRequest::InitializeNetParams();

        std::cout << testRequest.GetWeather();

    } catch (NRequest::TRException& excp) {
        std::cerr << excp.what() << std::endl;

    } catch (...) {
        std::cerr << "error was generated\n";
    }

    return 0;
}