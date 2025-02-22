cmake_minimum_required(VERSION 3.16...3.21)

project(Server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.3 COMPONENTS Core Network REQUIRED)
qt_standard_project_setup(REQUIRES 6.3)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Player.hpp
    src/Player.cpp
    src/Room.hpp
    src/Room.cpp
    src/Server.hpp
    src/Server.cpp
)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        OUTPUT_NAME RPSServer
)
target_link_libraries(${PROJECT_NAME} PRIVATE Common Qt6::Core Qt6::Network)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT DEPLOY_SCRIPT
)
install(SCRIPT ${DEPLOY_SCRIPT})
