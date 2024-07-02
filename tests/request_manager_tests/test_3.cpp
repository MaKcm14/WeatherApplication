#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TLogger logger;

TEST(Request_Manager_Tests, Empty_Data) {
    NRequest::TMRequest testRequest("");

    ASSERT_THROW(testRequest.GetWeather(), NRequest::TRException) << "the wrong exception"
    " type for empty data\n";

    try {
        testRequest.GetWeather();

    } catch (NRequest::TRException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 401) << "the wrong error id for empty data\n";;

    } catch (...) {
        FAIL() << "the unpredictable exception for empty data\n";
    }
}


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


TEST(Request_Manager_Tests, Uncorrect_Data_2) {
    NRequest::TMRequest testRequest;

    try {
        testRequest.GetWeather("K");

    } catch (NRequest::TRException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 401) << "the wrong error id for potential UB data\n";

    } catch (...) {
        FAIL() << "the unpredictable exception for potential UB data\n";
    } 
}


TEST(Request_Manager_Tests, Uncorrect_Data_3) {
    NRequest::TMRequest testRequest("Москва");
    try {
        testRequest.GetWeather();

    } catch (NRequest::TRException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 401) << "the wrong error id for uncorrect data\n";

    } catch (...) {
        FAIL() << "the unpredictable exception for uncorrect data\n";
    }
}


TEST(Request_Manager_Tests, Uninitialized_Data) {
    NRequest::TMRequest testRequest;

    try {
        testRequest.GetWeather();

    } catch (NRequest::TRException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 401) << "the wrong error id for uninitialized data\n";

    } catch (...) {
        FAIL() << "the unpredictable exception for uninitialized data\n";
    } 
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}