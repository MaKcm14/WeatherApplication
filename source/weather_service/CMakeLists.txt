# Request_manager list

include_directories(../cache_manager/include)
include_directories(../request_manager/include)

message("-- creating weather_service_lib target")
add_library(weather_service_lib STATIC weather_service.cpp)
