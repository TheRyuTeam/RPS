cmake_minimum_required(VERSION 3.16...3.21)

project(Client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Quick Network REQUIRED)

set(QT_QML_GENERATE_QMLLS_INI ON)

qt_standard_project_setup(REQUIRES 6.5)

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_executable(${PROJECT_NAME}
    src/main.cpp
)
qt_add_qml_module(${PROJECT_NAME}
    URI RPS
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/Header.qml
        qml/Menu.qml
        qml/CreateRoomMenu.qml
        qml/RoomsMenu.qml
        qml/SettingsMenu.qml
        qml/Room.qml
    SOURCES
        src/Client.hpp
        src/Client.cpp
    RESOURCES
        rc/img/exit.png
        rc/img/refresh.png
        rc/img/minus.png
        rc/img/checked.png
        rc/img/rock.png
        rc/img/scissors.png
        rc/img/paper.png
)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        OUTPUT_NAME RPSGame
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)
target_link_libraries(${PROJECT_NAME} PRIVATE Common Qt6::Quick Qt6::Network)
target_include_directories(${PROJECT_NAME} PRIVATE src)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT DEPLOY_SCRIPT
)
install(SCRIPT ${DEPLOY_SCRIPT})
