cmake_minimum_required(VERSION 3.16...3.21)

project(RPS VERSION 0.1)

find_package(Qt6 COMPONENTS Core REQUIRED)
qt_standard_project_setup(REQUIRES 6.5)

option(RPS_BUILD_CLIENT "Build client application" ON)
option(RPS_BUILD_SERVER "Build server application" ON)

if(RPS_BUILD_CLIENT OR RPS_BUILD_SERVER)
    add_subdirectory(Common)
endif()
if(RPS_BUILD_CLIENT)
    add_subdirectory(Client)
endif()
if(RPS_BUILD_SERVER)
    add_subdirectory(Server)
endif()
