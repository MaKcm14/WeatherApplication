# Request_manager_tests' list

include_directories(../source/request_manager/include)
include_directories(${GTest_INCLUDE_DIRS})

message("-- creating request_manager_tests' targets")

# Time_test
add_executable(r_m_time_test time_test.cpp)
target_link_libraries(r_m_time_test PRIVATE request_manager_lib PRIVATE logger_lib)
target_link_libraries(r_m_time_test PRIVATE gtest_main gtest)


# Test_1
add_executable(r_m_test_1 test_1.cpp)
target_link_libraries(r_m_test_1 PRIVATE request_manager_lib PRIVATE logger_lib)
target_link_libraries(r_m_test_1 PRIVATE gtest_main gtest)


# Test_2
add_executable(r_m_test_2 test_2.cpp)
target_link_libraries(r_m_test_2 PRIVATE request_manager_lib PRIVATE logger_lib)
target_link_libraries(r_m_test_2 PRIVATE gtest_main gtest)
