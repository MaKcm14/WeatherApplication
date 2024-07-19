# Request_manager_tests' list

include_directories(../source/cache_manager/include)
include_directories(${GTest_INCLUDE_DIRS})

message("-- creating cache_manager_tests' targets")

# Test_1
add_executable(c_m_test_1 test_1.cpp)
target_link_libraries(c_m_test_1 PRIVATE cache_manager_lib PRIVATE gtest_main gtest)


# Test_2
add_executable(c_m_test_2 test_2.cpp)
target_link_libraries(c_m_test_2 PRIVATE cache_manager_lib PRIVATE gtest_main gtest)


# Run_All_Tests_Together
add_executable(cm_run cache_manager_tests_run.cpp)
target_link_libraries(cm_run PRIVATE cache_manager_lib PRIVATE gtest_main gtest)
