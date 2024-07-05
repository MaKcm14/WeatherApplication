# Cache_manager list

include_directories(../logger/include)
include_directories(../request_manager/include)

message("-- creating cache_manager_lib target")
add_library(cache_manager_lib STATIC cache_manager.cpp ../request_manager/request_exception.cpp)
