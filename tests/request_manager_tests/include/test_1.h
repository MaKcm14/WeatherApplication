#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TEST(Request_Manager_Tests, City_Moscow) {
    NRequest::TRequestManager request;
    try {
        request.GetWeather("Moscow");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 505) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}


TEST(Request_Manager_Tests, City_Kazan) {
    NRequest::TRequestManager request;
    try {
        request.GetWeather("Kazan");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 505) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}


TEST(Request_Manager_Tests, City_Saint_Petersburg) {
    NRequest::TRequestManager request;
    try {
        request.GetWeather("Saint Petersburg");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 505) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}
