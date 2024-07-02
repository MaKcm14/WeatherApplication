#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TLogger logger;

TEST(Request_Manager_Tests, Uncorrect_Data_1) {
    NRequest::TMRequest testRequest("oiewbdibcjnl");
    try {
        testRequest.GetWeather();

    } catch (NRequest::TRException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 401) << "the wrong error id for uncorrect data\n";

    } catch (...) {
        FAIL() << "the unpredictable exception for uncorrect data\n";
    }
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}