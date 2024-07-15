#include "gtest/gtest.h"
#include "include/test_1.h"
#include "include/test_2.h"
#include "include/test_3.h"
#include "include/time_test.h"

TLogger logger;

int main(int argc, char* argv[]) {
    NRequest::ConfigureRequestService();
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}