#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TEST(Request_Manager_Tests, Time_Test) {
    NRequest::TRequestManager request;
    try {
        request.GetWeather("Moscow");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}
