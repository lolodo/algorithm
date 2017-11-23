cmake_minimum_required(VERSION 3.4)

project(avm_app VERSION 0.1)

set (SOURCES_common app_main.c gst_func.c
    libutils.c)

find_package(PkgConfig)
pkg_check_modules (GSTREAMER   gstreamer-1.0)

# add dependency libraries
set (LIBRARIES ${LIBRARIES}
    gstapp-1.0
    blacksesame
    ${GSTREAMER_LIBRARIES}
    )

add_executable (gst_appsrc gst_appsrc.c)
target_link_libraries(gst_appsrc ${LIBRARIES})
target_include_directories(gst_appsrc PUBLIC ${GSTREAMER_INCLUDE_DIRS})

add_executable (avm_blacksesame ${SOURCES_common} libavm_blacksesame.c)
target_link_libraries(avm_blacksesame ${LIBRARIES})
target_include_directories(avm_blacksesame PUBLIC ${GSTREAMER_INCLUDE_DIRS})

install (TARGETS gst_appsrc DESTINATION bin)
install (TARGETS avm_blacksesame DESTINATION bin)
