cmake_minimum_required(VERSION 3.5)

project(ShPlayer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()


add_subdirectory(lib/ffmpeg)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if(APPLE)
    list(APPEND EXTRA_LIBS
        "-framework OpenGL"
    )

elseif(WIN32)
    list(APPEND EXTRA_LIBS
        "-lglu32 -lopengl32"
    )
    set(CMAKE_EXE_LINKER_FLAGS "-std=gnu99 -static -static-libgcc -static-libstdc++ -mwindows")

else()
    list(APPEND EXTRA_LIBS
         "lGL -lGLU lX11"
    )

endif()

if(APPLE)
  add_executable(ShPlayer
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.ui
    src/ui/videorenderwidget.cpp
    src/ui/videorenderwidget.h
    src/videoplayer.cpp
    src/videoplayer.h
  )
endif()

target_link_libraries(ShPlayer ffmpeg Qt5::Widgets ${EXTRA_LIBS} )
