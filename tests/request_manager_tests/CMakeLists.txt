# Request_manager_tests' list

include_directories(../source/request_manager/include)
include_directories(${GTest_INCLUDE_DIRS})

message("-- creating request_manager_tests' targets")

# Time_test
add_executable(r_m_time_test time_test.cpp)
target_link_libraries(r_m_time_test PRIVATE request_manager_lib PRIVATE gtest_main gtest)


# Test_1
add_executable(r_m_test_1 test_1.cpp)
target_link_libraries(r_m_test_1 PRIVATE request_manager_lib PRIVATE gtest_main gtest)


# Test_2
add_executable(r_m_test_2 test_2.cpp)
target_link_libraries(r_m_test_2 PRIVATE request_manager_lib PRIVATE gtest_main gtest)


# Test_3
add_executable(r_m_test_3 test_3.cpp)
target_link_libraries(r_m_test_3 PRIVATE request_manager_lib PRIVATE gtest_main gtest)


# Run_All_Tests_Together
add_executable(rm_run request_manager_tests_run.cpp)
target_link_libraries(rm_run PRIVATE request_manager_lib PRIVATE gtest_main gtest)
