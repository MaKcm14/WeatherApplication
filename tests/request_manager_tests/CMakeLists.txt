# Request_manager_tests' list

include_directories(../source/request_manager/include)
include_directories(${GTest_INCLUDE_DIRS})

message("-- create request_manager_tests' targets")
add_executable(r_m_test_1 test_1.cpp)

target_link_libraries(r_m_test_1 PRIVATE request_manager_lib PRIVATE logger_lib)
target_link_libraries(r_m_test_1 PRIVATE ${GTest_INCLUDE_LIBRARIES})