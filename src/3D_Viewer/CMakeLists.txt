cmake_minimum_required(VERSION 3.5)

project(3D_Viewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${Qt6Widgets_INCLUDE_DIRS} ${Qt6OpenGLWidgets_INCLUDE_DIRS})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(OpenGL REQUIRED)

#set(QTGIFIMAGE_DIR "./QtGifImage/src")

set(QT_OPENGL_WIDGETS_MODULE "OpenGLWidgets")

if(QT_VERSION_MAJOR EQUAL 5)
    find_package(Qt5OpenGLWidgets QUIET)
    if(NOT Qt5OpenGLWidgets_FOUND)
        set(QT_OPENGL_WIDGETS_MODULE "OpenGL")
    endif()
endif()
add_compile_definitions(QT_OPENGL_WIDGETS)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        glwidget.cpp
        glwidget.h
        ../backend/readobj.c
        ../backend/readobj.h
        libgif/gif.cc
        libgif/gif.h
  #      ./QtGifImage/src/3rdParty/giflib/gif_err.c
  #      ./QtGifImage/src/3rdParty/giflib/dgif_lib.c
   #     ./QtGifImage/src/3rdParty/giflib/egif_lib.c
   #     ./QtGifImage/src/3rdParty/giflib/gif_hash.c
   #     ./QtGifImage/src/3rdParty/giflib/gifalloc.c
  #      ./QtGifImage/src/3rdParty/giflib/quantize.c
   #     ./QtGifImage/src/gifimage/qgifimage.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(3D_Viewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(3D_Viewer SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(3D_Viewer
            ${PROJECT_SOURCES}
        )
    endif()
endif()
target_link_libraries(3D_Viewer PRIVATE Qt6::Widgets Qt6::OpenGLWidgets)

set_target_properties(3D_Viewer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(3D_Viewer)
endif()
