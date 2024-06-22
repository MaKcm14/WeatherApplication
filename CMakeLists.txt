cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(WeatherWebApplication)

# Find some packages
message("-- find boost-1.55.0 and postgresql-10.0")
find_package(Boost 1.55.0 COMPONENTS system regex REQUIRED)
find_package(PostgreSQL 10.0)

if (Boost_FOUND)
    message("-- boost: success")
    #include_directories(${Boost_INCLUDE_DIRS})
    #target_link_libraries(main PRIVATE ${Boost_INCLUDE_LIBRARIES})
endif()

#if (PostgreSQL)
#    message("-- postgresql: success")
    #include_directories(${PostgreSQL})
    #target_link_libraries(main PRIVATE ${PostgreSQL_INCLUDE_LIBRARIES})
#endif()


# Tests part
if (ENABLE_TEST)
    enable_testing()
    message("-- enable testing")
endif()

add_test(Test_1 test_1)
add_test(Test_2 test_2)
