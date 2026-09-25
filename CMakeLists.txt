cmake_minimum_required(VERSION 4.3.0)

project(HttpClient VERSION 1.0.0 LANGUAGES CXX)

add_library(HttpClient STATIC
    src/HttpClient.cpp
)

target_include_directories(HttpClient
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(HttpClient PUBLIC cxx_std_20)

target_link_libraries(HttpClient
    PUBLIC
        winhttp
)

set_target_properties(HttpClient PROPERTIES
    OUTPUT_NAME "winhttp"
    DEBUG_POSTFIX "d"
)