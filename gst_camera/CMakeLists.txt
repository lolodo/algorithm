cmake_minimum_required(VERSION 3.4)

project(gst_camera VERSION 0.1)

set (SOURCES_common get_streaming_video.c)

find_package(PkgConfig)
pkg_check_modules(GSTREAMER   gstreamer-1.0)

# add dependency libraries
set (LIBRARIES ${LIBRARIES}
    gstapp-1.0
    pthread
    ${GSTREAMER_LIBRARIES}
    )

add_executable (camera ${SOURCES_common})
target_link_libraries(camera ${LIBRARIES})
target_include_directories(camera PUBLIC ${GSTREAMER_INCLUDE_DIRS})

install (TARGETS camera DESTINATION bin)
