cmake_minimum_required(VERSION 3.4)

project(avm_app VERSION 0.1)

set (SOURCES_common app_main.c gst_func.c
    libutils.c)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

find_package(PkgConfig)
pkg_check_modules (GSTREAMER   gstreamer-1.0)

# add dependency libraries
set (LIBRARIES ${LIBRARIES}
    gstapp-1.0
	pthread
    ${GSTREAMER_LIBRARIES}
    )

add_executable (avm_blacksesame ${SOURCES_common})
target_link_libraries(avm_blacksesame ${LIBRARIES})
target_include_directories(avm_blacksesame PUBLIC ${GSTREAMER_INCLUDE_DIRS})

install (TARGETS avm_blacksesame DESTINATION bin)
