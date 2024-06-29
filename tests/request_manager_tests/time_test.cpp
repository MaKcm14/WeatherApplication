#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TLogger logger;

TEST(Request_Manager_Tests, Time_Test) {
    NRequest::TMRequest testRequest1("Moscow");

    for (bool noThrow = false; !noThrow; ) {
        try {
            testRequest1.GetWeather();
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

}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}