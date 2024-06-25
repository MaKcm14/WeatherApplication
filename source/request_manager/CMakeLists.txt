# request_manager list

include_directories(./include)

message("-- create request_manager_lib target")
add_library(request_manager_lib STATIC request_manager.cpp request_exception.cpp)
