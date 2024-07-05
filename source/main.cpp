#include "logger.h"
#include "cache_manager.h"

TLogger logger;

int main() {
    //NRequest::TRequestManager request("Moscow");

    // try {
    //      std::cout << request.GetWeather();
    // } catch (NRequest::TRequestException& excp) {
    //     std::cerr << excp.what();
    // } catch (...) {
        
    // }

    try {
        NRequest::InitConfig();
        NRequest::TCacheManager manager;
    } catch (...) {
        return 1;   
    }

    return 0;
}