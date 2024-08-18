#include "gtest/gtest.h"
#include "logger.h"
#include "request_manager.h"

TEST(Request_Manager_Tests, Empty_Data) {
    NRequest::TRequestManager request;

    ASSERT_THROW(request.GetWeatherTemplate(""), NRequest::TRequestException) << "the wrong exception"
    " type for empty data\n";

    try {
        request.GetWeatherTemplate("");

    } catch (NRequest::TRequestException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 401) << "the wrong error id for empty data\n";;

    } catch (...) {
        FAIL() << "the unpredictable exception for empty data\n";
    }
}


TEST(Request_Manager_Tests, Uncorrect_Data_1) {
    NRequest::TRequestManager request;
    try {
        request.GetWeatherTemplate("oiewbdibcjnl");

    } catch (NRequest::TRequestException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 404) << "the wrong error id for uncorrect data\n";

    } catch (...) {
        FAIL() << "the unpredictable exception for uncorrect data\n";
    }
}


TEST(Request_Manager_Tests, Uncorrect_Data_2) {
    NRequest::TRequestManager request;
    try {
        request.GetWeatherTemplate("K");

    } catch (NRequest::TRequestException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 404) << "the wrong error id for potential UB data\n";

    } catch (...) {
        FAIL() << "the unpredictable exception for potential UB data\n";
    } 
}


TEST(Request_Manager_Tests, Uncorrect_Data_3) {
    NRequest::TRequestManager request;
    try {
        request.GetWeatherTemplate("Москва");

    } catch (NRequest::TRequestException& excp) {
        ASSERT_EQ(excp.GetErrorId(), 404) << "the wrong error id for uncorrect data\n" << excp.what();

    } catch (...) {
        FAIL() << "the unpredictable exception for uncorrect data\n";
    }
}

