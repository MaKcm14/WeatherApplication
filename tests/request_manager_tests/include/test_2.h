#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TEST(Request_Manager_Tests, Town_Bugulma) {
    NRequest::TRequestManager request;
    try {
        request.GetWeatherTemplate("Bugulma");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}


TEST(Request_Manager_Tests, Town_Almetyevsk) {
    NRequest::TRequestManager request;
    try {
        request.GetWeatherTemplate("Almetyevsk");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}


TEST(Request_Manager_Tests, Town_Nizhnekamsk) {
    NRequest::TRequestManager request;
    try {
        request.GetWeatherTemplate("Nizhnekamsk");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}
