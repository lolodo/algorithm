cmake_minimum_required(VERSION 3.4)
project(rtph264 VERSION 0.1)

include_directories(/usr/include/glib-2.0/)
include_directories(./socket)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)

find_package(PkgConfig)
find_package(OpenCV REQUIRED)

add_executable(rtpreceiver rtpreceiver.cpp RtpH264Depay.cpp)
target_link_libraries(rtpreceiver jrtp ${OpenCV_LIBS} glib-2.0 socket)

install (TARGETS rtpreceiver DESTINATION bin)
add_subdirectory(socket)
