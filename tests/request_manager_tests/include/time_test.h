#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TEST(Request_Manager_Tests, Time_Test) {
    NRequest::TRequestManager request;
    try {
        request.GetWeatherTemplate("Moscow");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}
