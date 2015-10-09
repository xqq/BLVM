cmake_minimum_required(VERSION 3.2)
project(bli)

set(SOURCE_FILES main.cpp)
add_executable(bli ${SOURCE_FILES})
target_link_libraries(bli BLVM)