cmake_minimum_required(VERSION 3.4)
project(rtph264 VERSION 0.1)
#set(glib_DIR /)

#include_directories(./rtph264depay)
include_directories(./h264depay)
include_directories(/usr/include/glib-2.0/)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)

#find_package(GTK2)
#if (NOT GTK2_FOUND)
#    message(FATAL_ERROR "GTK2 Not Found!")
#endif (NOT GTK2_FOUND)

#include_directories(${GTK2_INCLUDE_DIRS})

find_package(PkgConfig)
find_package(OpenCV REQUIRED)

add_executable(rtpreceiver rtpreceiver.cpp)
#target_link_libraries(rtpreceiver jrtp ${OpenCV_LIBS} ${GTK2_LIBRARIES} rtph264depay)
target_link_libraries(rtpreceiver jrtp ${OpenCV_LIBS} glib-2.0 rtph264depay)

install (TARGETS rtpreceiver DESTINATION bin)
#add_subdirectory(rtph264depay)
add_subdirectory(h264depay)
