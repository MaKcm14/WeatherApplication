#include "request_manager.cpp"

int main() {
    NRequest::TMRequest testRequest("Moscow");

    NRequest::InitializeNetParams();

    std::cout << testRequest.GetWeather();

    return 0;
}