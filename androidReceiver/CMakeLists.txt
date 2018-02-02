cmake_minimum_required(VERSION 3.4)
project(rtph264 VERSION 0.1)

find_package(PkgConfig)

find_package(OpenCV REQUIRED)
add_executable(rtpreceiver rtpreceiver.cpp)
target_link_libraries(rtpreceiver jrtp ${OpenCV_LIBS})

install (TARGETS rtpreceiver DESTINATION bin)
