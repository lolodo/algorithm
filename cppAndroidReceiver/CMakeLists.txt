cmake_minimum_required(VERSION 3.4)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/helper/")
project(rtph264 VERSION 0.1)

include_directories(/usr/include/glib-2.0/)
include_directories(/usr/local/include)
include_directories(./socket)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)
include_directories(/usr/include/x86_64-linux-gnu)

find_package(PkgConfig)
if (NOT PkgConfig_FOUND)
	message (FATAL_ERROR "PkgConfig not found!")
endif (NOT PkgConfig_FOUND)

find_package(OpenCV REQUIRED)
if (NOT OpenCV_FOUND)
	message (FATAL_ERROR "OpenCV not found!")
endif (NOT OpenCV_FOUND)

add_executable(rtpreceiver rtpreceiver.cpp RtpH264Depay.cpp)
target_link_libraries(rtpreceiver jrtp ${OpenCV_LIBS} glib-2.0 avformat avcodec avutil m z socket)

install (TARGETS rtpreceiver DESTINATION bin)
add_subdirectory(socket)
