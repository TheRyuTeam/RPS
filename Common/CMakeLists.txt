cmake_minimum_required(VERSION 3.16...3.21)

project(Common LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8 COMPONENTS Core Network REQUIRED)
qt_standard_project_setup(REQUIRES 6.8)

add_library(${PROJECT_NAME} STATIC
    include/RPS/Common.hpp
    src/Common.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Network)
target_compile_definitions(${PROJECT_NAME} PRIVATE COMMON_LIBRARY)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME RPSCommon)
target_include_directories(Common PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
