#SET(CMAKE_C_COMPILER "/usr/bin/gcc")
#find_library(GLIB_CORE glib-2.0 /usr/lib)
#link_libraries($(GLIB_CORE))
add_library(rtph264depay STATIC rtph264depay.c)
