#include "logger.h"
#include "request_manager.h"

TLogger logger;

int main() {
    NRequest::TMRequest request("Saint Petersburg");

    try {
         std::cout << request.GetWeather();
    } catch (NRequest::TRException& excp) {
        std::cerr << excp.what();
    } catch (...) {
        
    }

    return 0;
}