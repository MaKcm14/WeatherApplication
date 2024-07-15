#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TEST(Request_Manager_Tests, Town_Bugulma) {
    NRequest::TRequestManager request;
    try {
        request.GetWeather("Bugulma");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}


TEST(Request_Manager_Tests, Town_Almetyevsk) {
    NRequest::TRequestManager request;
    try {
        request.GetWeather("Almetyevsk");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}


TEST(Request_Manager_Tests, Town_Nizhnekamsk) {
    NRequest::TRequestManager request;
    try {
        request.GetWeather("Nizhnekamsk");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}
