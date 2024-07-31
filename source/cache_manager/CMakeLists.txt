# Cache_manager list

include_directories(../logger/include)
include_directories(../request_manager/include)

message("-- creating data_base_lib")
add_library(data_base_lib STATIC data_base.cpp data_base_exception.cpp)

message("-- creating cache_manager_lib target")
add_library(cache_manager_lib STATIC cache_manager.cpp ../request_manager/request_exception.cpp)
target_link_libraries(cache_manager_lib PUBLIC data_base_lib)
