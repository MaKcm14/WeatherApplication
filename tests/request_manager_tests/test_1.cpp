#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TLogger logger;

TEST(Request_Manager_Tests, City_Moscow) {
    NRequest::TMRequest testRequest1("Moscow");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("Moscow");
            noThrow = true;

        } catch (NRequest::TRException& excp) {
            std::cerr << excp.what() << std::endl;
            if (excp.GetErrorId() >= 401 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
                ADD_FAILURE();
            }

        } catch (...) {
            ADD_FAILURE();
        }
    }
    ASSERT_NE(testRequest1.GetWeather().size(), 0);
    ASSERT_NE(testRequest2.GetWeather().size(), 0);

    ASSERT_STREQ(testRequest1.GetWeather().c_str(), testRequest2.GetWeather().c_str());
}


TEST(Request_Manager_Tests, City_Kazan) {
    NRequest::TMRequest testRequest1("Kazan");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("Kazan");
            noThrow = true;

        } catch (NRequest::TRException& excp) {
            std::cerr << excp.what() << std::endl;
            if (excp.GetErrorId() >= 401 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
                ADD_FAILURE();
            }

        } catch (...) {
            ADD_FAILURE();
        }
    }
    ASSERT_NE(testRequest1.GetWeather().size(), 0);
    ASSERT_NE(testRequest2.GetWeather().size(), 0);

    ASSERT_STREQ(testRequest1.GetWeather().c_str(), testRequest2.GetWeather().c_str());
}


TEST(Request_Manager_Tests, DISABLED_City_Saint_Petersburg) {
    NRequest::TMRequest testRequest1("SaintPetersburg");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("SaintPetersburg");
            noThrow = true;

        } catch (NRequest::TRException& excp) {
            std::cerr << excp.what() << std::endl;
            if (excp.GetErrorId() >= 401 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
                ADD_FAILURE();
            }

        } catch (...) {
            ADD_FAILURE();
        }
    }
    ASSERT_NE(testRequest1.GetWeather().size(), 0);
    ASSERT_NE(testRequest2.GetWeather().size(), 0);

    ASSERT_STREQ(testRequest1.GetWeather().c_str(), testRequest2.GetWeather().c_str());
}


TEST(Request_Manager_Tests, City_Yekaterinburg) {
    NRequest::TMRequest testRequest1("Yekaterinburg");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("Yekaterinburg");
            noThrow = true;

        } catch (NRequest::TRException& excp) {
            std::cerr << excp.what() << std::endl;
            if (excp.GetErrorId() >= 401 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
                ADD_FAILURE();
            }

        } catch (...) {
            ADD_FAILURE();
        }
    }
    ASSERT_NE(testRequest1.GetWeather().size(), 0);
    ASSERT_NE(testRequest2.GetWeather().size(), 0);

    ASSERT_STREQ(testRequest1.GetWeather().c_str(), testRequest2.GetWeather().c_str());
}


TEST(Request_Manager_Tests, City_Novosibirsk) {
    NRequest::TMRequest testRequest1("Novosibirsk");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("Novosibirsk");
            noThrow = true;

        } catch (NRequest::TRException& excp) {
            std::cerr << excp.what() << std::endl;
            if (excp.GetErrorId() >= 401 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
                ADD_FAILURE();
            }

        } catch (...) {
            ADD_FAILURE();
        }
    }
    ASSERT_NE(testRequest1.GetWeather().size(), 0);
    ASSERT_NE(testRequest2.GetWeather().size(), 0);

    ASSERT_STREQ(testRequest1.GetWeather().c_str(), testRequest2.GetWeather().c_str());
}


TEST(Request_Manager_Tests, City_Ulan_Ude) {
    NRequest::TMRequest testRequest1("Ulan-Ude");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("Ulan-Ude");
            noThrow = true;

        } catch (NRequest::TRException& excp) {
            std::cerr << excp.what() << std::endl;
            if (excp.GetErrorId() >= 401 && excp.GetErrorId() <= 499 || excp.GetErrorId() == 501) {
                ADD_FAILURE();
            }

        } catch (...) {
            ADD_FAILURE();
        }
    }
    ASSERT_NE(testRequest1.GetWeather().size(), 0);
    ASSERT_NE(testRequest2.GetWeather().size(), 0);

    ASSERT_STREQ(testRequest1.GetWeather().c_str(), testRequest2.GetWeather().c_str());
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}