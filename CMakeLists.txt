cmake_minimum_required(VERSION 3.14)

project(ShPlayer C CXX)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


#set(CMAKE_PREFIX_PATH "C:/Qt/5.11.1/msvc2015_64")


add_subdirectory(lib/ffmpeg)

find_package(Qt5 COMPONENTS OpenGL Gui Core Widgets REQUIRED)

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


add_executable(ShPlayer
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.ui
    src/videoplayer.cpp
    src/videoplayer.h
    src/shplayer-ffmpeg/demuxer.cpp
    src/shplayer-ffmpeg/demuxer.h
    src/shplayer-ffmpeg/packet.cpp
    src/shplayer-ffmpeg/packet.h
    src/shplayer-ffmpeg/videodecoder.cpp
    src/shplayer-ffmpeg/videodecoder.h
    src/shplayer-ffmpeg/codecinfo.cpp
    src/shplayer-ffmpeg/codecinfo.h
    src/shplayer-ffmpeg/frame.cpp
    src/shplayer-ffmpeg/frame.h
    src/ui/videorenderwidget.cpp
    src/ui/videorenderwidget.h
    #src/ui/videorenderwidget2.cpp
    #src/ui/videorenderwidget2.h
 )


target_link_libraries(ShPlayer ffmpeg Qt5::OpenGL Qt5::Gui Qt5::Core Qt5::Widgets ${EXTRA_LIBS} )
