# Set main info
cmake_minimum_required(VERSION 3.1...3.28 FATAL_ERROR)

project(WeatherWebApplication)

set_target_properties(${TARGET} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Set some variables
set(REQUEST_MANAGER_DIR ./source/request_manager)
set(LOGGER_DIR ./source/logger)

# Create main targets
include_directories(./source/request_manager/include)
include_directories(./source/logger/include)

add_subdirectory(${LOGGER_DIR})
add_subdirectory(${REQUEST_MANAGER_DIR})

message("-- create main target")
add_executable(main ./source/main.cpp)

target_link_libraries(request_manager_lib PUBLIC logger_lib)
target_link_libraries(main PRIVATE request_manager_lib)


# Find packages
set(nlohmann_json_DIR ./source/nlohmann_json/build)

message("-- find packages for Boost-1.55.0 and upper")
find_package(Boost 1.55.0 COMPONENTS system regex REQUIRED)

message("-- find packages for PostgreSQL-10.0 and upper")
find_package(PostgreSQL 10.0)

message("-- find packages for nlohmann_json-3.1.0 and upper")
find_package(nlohmann_json 3.1.0)


# Link found packages
if (Boost_FOUND)
    message("-- packages for Boost-1.55.0 and upper were found")
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(request_manager_lib PUBLIC ${Boost_INCLUDE_LIBRARIES})
endif()

if (PostgreSQL_FOUND)
    message("-- packages for PostgreSQL-10.0 and upper were found")
    include_directories(${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(main PRIVATE PostgreSQL::PostgreSQL)
endif()

if (nlohmann_json_FOUND)
    message("-- packages for nlohmann_json-3.1.0 and upper were found")
    include_directories(${nlohmann_json_INCLUDE_DIRS})
    target_link_libraries(request_manager_lib PUBLIC ${nlohmann_json_INCLUDE_LIBRARIES})
endif()

# TODO: make optimization for all CMakeLists.txt by separating the finding parts (after all managers will be completed)

# Tests part
if (ENABLE_TEST)
    message("-- enable testing for WeatherWebApplication")
    enable_testing()
endif()

