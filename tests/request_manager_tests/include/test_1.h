#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TEST(Request_Manager_Tests, City_Moscow) {
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


TEST(Request_Manager_Tests, City_Kazan) {
    NRequest::TRequestManager request;
    try {
        request.GetWeatherTemplate("Kazan");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}


TEST(Request_Manager_Tests, City_Saint_Petersburg) {
    NRequest::TRequestManager request;
    try {
        request.GetWeatherTemplate("Saint Petersburg");

    } catch (NRequest::TRequestException& excp) {
        std::cerr << excp.what() << std::endl;
        if (excp.GetErrorId() >= 400 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
            FAIL();
        }

    } catch (...) {
        FAIL();
    }
}
