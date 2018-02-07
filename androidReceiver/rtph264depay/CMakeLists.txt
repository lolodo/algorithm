set (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}") 
add_library(rtph264depay STATIC RtpH264Depay.cpp)
