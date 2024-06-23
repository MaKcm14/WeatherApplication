# Set main info
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(WeatherWebApplication)

set_target_properties(${TARGET} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)


# Create main targets


# Find packages
set(nlohmann_json_DIR /usr/include/json/build)

message("-- find packages for Boost-1.55.0 and upper")
find_package(Boost 1.55.0 COMPONENTS system regex REQUIRED)

message("-- find packages for PostgreSQL-10.0 and upper")
find_package(PostgreSQL 10.0)

message("-- find packages for nlohmann_json-3.1.0 and upper")
find_package(nlohmann_json 3.1.0)


# Link found packages
if (Boost_FOUND)
    message("-- packages for Boost-1.55.0 and upper were found")
    #include_directories(${Boost_INCLUDE_DIRS})
    #target_link_libraries(main PRIVATE ${Boost_INCLUDE_LIBRARIES})
endif()

if (PostgreSQL_FOUND)
    message("-- packages for PostgreSQL-10.0 and upper were found")
    #include_directories(${PostgreSQL_INCLUDE_DIRS})
    #target_link_libraries(main PRIVATE PostgreSQL::PostgreSQL)
endif()

if (nlohmann_json_FOUND)
    message("-- packages for nloBoost-1.55.0hmann_json-3.1.0 and upper were found")
    #include_directories(${nlohmann_json_INCLUDE_DIRS})
    #target_link_libraries(main PRIVATE ${nlohmann_json_INCLUDE_LIBRARIES})
endif()


# Tests part
if (ENABLE_TEST)
    message("-- enable testing for WeatherWebApplication")
    enable_testing()
endif()

