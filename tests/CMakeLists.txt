# Main list for tests

# Request_manager's tests
add_subdirectory(./request_manager_tests)

add_test(NAME request_manager_test_time COMMAND r_m_time_test)

add_test(NAME request_manager_test_1 COMMAND r_m_test_1)

add_test(NAME request_manager_test_2 COMMAND r_m_test_2)
