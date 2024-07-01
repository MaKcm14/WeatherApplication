#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TLogger logger;

TEST(Request_Manager_Tests, Town_Bugulma) {
    NRequest::TMRequest testRequest1("Bugulma");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("Bugulma");
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


TEST(Request_Manager_Tests, Town_Sochi) {
    NRequest::TMRequest testRequest1("Sochi");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("Sochi");
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


TEST(Request_Manager_Tests, Town_Almetyevsk) {
    NRequest::TMRequest testRequest1("Almetyevsk");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("Almetyevsk");
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


TEST(Request_Manager_Tests, Town_Nizhnekamsk) {
    NRequest::TMRequest testRequest1("Nizhnekamsk");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("Nizhnekamsk");
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


TEST(Request_Manager_Tests, Town_Tver) {
    NRequest::TMRequest testRequest1("Tver");
    NRequest::TMRequest testRequest2;

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
            testRequest2.GetWeather("Tver");
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