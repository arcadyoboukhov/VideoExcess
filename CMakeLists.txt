cmake_minimum_required(VERSION 3.16)

project(VideoExcess VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the src directory to the path for source files
set(SOURCE_DIR src)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

set(PROJECT_SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/mainwindow.cpp
    ${SOURCE_DIR}/mainwindow.h
    ${SOURCE_DIR}/settings.cpp
    ${SOURCE_DIR}/settings.h
    ${SOURCE_DIR}/form.cpp
    ${SOURCE_DIR}/form.h
    ${SOURCE_DIR}/player.cpp
    ${SOURCE_DIR}/player.h
    ${SOURCE_DIR}/mainwindow.ui
    ${SOURCE_DIR}/settings.ui
    ${SOURCE_DIR}/form.ui
    ${SOURCE_DIR}/player.ui
    ${SOURCE_DIR}/filex.cpp
    ${SOURCE_DIR}/filex.h
    ${SOURCE_DIR}/filex.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VideoExcess  # Changed this line
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        assetsConfig.qrc
    )
else()
    if(ANDROID)
        add_library(VideoExcess SHARED  # Also change here for Android
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(VideoExcess  # Changed this line
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(VideoExcess PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.VideoExcess)  # Change this line too
endif()

set_target_properties(VideoExcess PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS VideoExcess  # Change this line
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VideoExcess)  # Change this line
endif()
