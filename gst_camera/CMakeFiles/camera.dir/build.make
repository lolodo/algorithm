# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/fangyuan/workspace/github/algorithm/gst_camera

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fangyuan/workspace/github/algorithm/gst_camera

# Include any dependencies generated for this target.
include CMakeFiles/camera.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/camera.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/camera.dir/flags.make

CMakeFiles/camera.dir/get_streaming_video.c.o: CMakeFiles/camera.dir/flags.make
CMakeFiles/camera.dir/get_streaming_video.c.o: get_streaming_video.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fangyuan/workspace/github/algorithm/gst_camera/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/camera.dir/get_streaming_video.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/camera.dir/get_streaming_video.c.o   -c /home/fangyuan/workspace/github/algorithm/gst_camera/get_streaming_video.c

CMakeFiles/camera.dir/get_streaming_video.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/camera.dir/get_streaming_video.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/fangyuan/workspace/github/algorithm/gst_camera/get_streaming_video.c > CMakeFiles/camera.dir/get_streaming_video.c.i

CMakeFiles/camera.dir/get_streaming_video.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/camera.dir/get_streaming_video.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/fangyuan/workspace/github/algorithm/gst_camera/get_streaming_video.c -o CMakeFiles/camera.dir/get_streaming_video.c.s

CMakeFiles/camera.dir/get_streaming_video.c.o.requires:

.PHONY : CMakeFiles/camera.dir/get_streaming_video.c.o.requires

CMakeFiles/camera.dir/get_streaming_video.c.o.provides: CMakeFiles/camera.dir/get_streaming_video.c.o.requires
	$(MAKE) -f CMakeFiles/camera.dir/build.make CMakeFiles/camera.dir/get_streaming_video.c.o.provides.build
.PHONY : CMakeFiles/camera.dir/get_streaming_video.c.o.provides

CMakeFiles/camera.dir/get_streaming_video.c.o.provides.build: CMakeFiles/camera.dir/get_streaming_video.c.o


# Object files for target camera
camera_OBJECTS = \
"CMakeFiles/camera.dir/get_streaming_video.c.o"

# External object files for target camera
camera_EXTERNAL_OBJECTS =

camera: CMakeFiles/camera.dir/get_streaming_video.c.o
camera: CMakeFiles/camera.dir/build.make
camera: CMakeFiles/camera.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fangyuan/workspace/github/algorithm/gst_camera/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable camera"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/camera.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/camera.dir/build: camera

.PHONY : CMakeFiles/camera.dir/build

# Object files for target camera
camera_OBJECTS = \
"CMakeFiles/camera.dir/get_streaming_video.c.o"

# External object files for target camera
camera_EXTERNAL_OBJECTS =

CMakeFiles/CMakeRelink.dir/camera: CMakeFiles/camera.dir/get_streaming_video.c.o
CMakeFiles/CMakeRelink.dir/camera: CMakeFiles/camera.dir/build.make
CMakeFiles/CMakeRelink.dir/camera: CMakeFiles/camera.dir/relink.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fangyuan/workspace/github/algorithm/gst_camera/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable CMakeFiles/CMakeRelink.dir/camera"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/camera.dir/relink.txt --verbose=$(VERBOSE)

# Rule to relink during preinstall.
CMakeFiles/camera.dir/preinstall: CMakeFiles/CMakeRelink.dir/camera

.PHONY : CMakeFiles/camera.dir/preinstall

CMakeFiles/camera.dir/requires: CMakeFiles/camera.dir/get_streaming_video.c.o.requires

.PHONY : CMakeFiles/camera.dir/requires

CMakeFiles/camera.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/camera.dir/cmake_clean.cmake
.PHONY : CMakeFiles/camera.dir/clean

CMakeFiles/camera.dir/depend:
	cd /home/fangyuan/workspace/github/algorithm/gst_camera && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fangyuan/workspace/github/algorithm/gst_camera /home/fangyuan/workspace/github/algorithm/gst_camera /home/fangyuan/workspace/github/algorithm/gst_camera /home/fangyuan/workspace/github/algorithm/gst_camera /home/fangyuan/workspace/github/algorithm/gst_camera/CMakeFiles/camera.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/camera.dir/depend
