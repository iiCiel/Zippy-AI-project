cmake_minimum_required(VERSION 3.16)

project(cob_zippy_ai VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Core)

qt_standard_project_setup(REQUIRES 6.8)

set(CMAKE_AUTORCC ON)

# Include directory for headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Collect all source and header files
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)

qt_add_executable(appcob_zippy_ai
    ${SOURCES}
    ${HEADERS}
    resources.qrc
)

qt_add_qml_module(appcob_zippy_ai
    URI cob_zippy_ai
    VERSION 1.0

    QML_FILES
        qml/Main.qml
        qml/OllamaConfig.qml
        qml/Events.qml
        qml/BuildingMaps.qml
        qml/Contact.qml

    RESOURCES
        resources.qrc
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appcob_zippy_ai PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appcob_zippy_ai
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appcob_zippy_ai
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appcob_zippy_ai
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
