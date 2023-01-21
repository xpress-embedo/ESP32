cmake_minimum_required(VERSION 3.16)

project(MQTT_Application VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)
#find_package(Qt6 REQUIRED COMPONENTS Mqtt)

qt_add_executable(appMQTT_Application
    main.cpp
    someclass.h someclass.cpp
)

qt_add_qml_module(appMQTT_Application
    URI MQTT_Application
    VERSION 1.0
    QML_FILES main.qml 
)

set_target_properties(appMQTT_Application PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appMQTT_Application
    PRIVATE Qt6::Quick)

#target_link_libraries(appMQTT_Application
#    PRIVATE Qt6::Mqtt)

install(TARGETS appMQTT_Application
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
