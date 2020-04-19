cmake_minimum_required(VERSION 3.14)
project(ffmpeg)



if(APPLE)

    add_library(ffmpeg INTERFACE IMPORTED GLOBAL)

    target_include_directories(ffmpeg INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include
    )


    target_link_options(ffmpeg INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libavcodec.58.77.101.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libavformat.58.42.100.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libavfilter.7.77.101.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libavdevice.58.9.103.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libavutil.56.42.102.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libpostproc.55.6.100.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libswresample.3.6.100.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libswscale.5.6.101.dylib
    )



elseif(WIN32)

    add_library(ffmpeg INTERFACE IMPORTED GLOBAL)

    target_include_directories(ffmpeg INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-win/include
    )

    target_link_libraries(ffmpeg INTERFACE

    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-win/lib/avcodec.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-win/lib/avformat.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-win/lib/avfilter.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-win/lib/avdevice.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-win/lib/avutil.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-win/lib/postproc.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-win/lib/swresample.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-win/lib/swscale.lib

    )

else()

endif()
