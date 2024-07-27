#include "include/test_2.h"

TLogger logger;

namespace NWeather {
    std::mutex serviceMut;
}

int main(int argc, char* argv[]) {
    NRequest::ConfigureRequestService();
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}