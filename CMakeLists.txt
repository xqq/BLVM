cmake_minimum_required(VERSION 3.2)
project(BLVM)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL")
endif()

aux_source_directory(src SOURCE_FILES)
add_library(BLVM STATIC ${SOURCE_FILES})

add_subdirectory(tools/bli)