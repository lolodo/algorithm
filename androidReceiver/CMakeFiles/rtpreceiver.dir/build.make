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
CMAKE_SOURCE_DIR = /home/fangyuan/workspace/github/algorithm/androidReceiver

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fangyuan/workspace/github/algorithm/androidReceiver

# Include any dependencies generated for this target.
include CMakeFiles/rtpreceiver.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/rtpreceiver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/rtpreceiver.dir/flags.make

CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o: CMakeFiles/rtpreceiver.dir/flags.make
CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o: rtpreceiver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fangyuan/workspace/github/algorithm/androidReceiver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o -c /home/fangyuan/workspace/github/algorithm/androidReceiver/rtpreceiver.cpp

CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fangyuan/workspace/github/algorithm/androidReceiver/rtpreceiver.cpp > CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.i

CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fangyuan/workspace/github/algorithm/androidReceiver/rtpreceiver.cpp -o CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.s

CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o.requires:

.PHONY : CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o.requires

CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o.provides: CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o.requires
	$(MAKE) -f CMakeFiles/rtpreceiver.dir/build.make CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o.provides.build
.PHONY : CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o.provides

CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o.provides.build: CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o


# Object files for target rtpreceiver
rtpreceiver_OBJECTS = \
"CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o"

# External object files for target rtpreceiver
rtpreceiver_EXTERNAL_OBJECTS =

rtpreceiver: CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o
rtpreceiver: CMakeFiles/rtpreceiver.dir/build.make
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_videostab.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_ts.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_superres.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_ocl.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_gpu.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_contrib.so.2.4.9
rtpreceiver: rtph264depay/librtph264depay.a
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_photo.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_legacy.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_video.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_ml.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_flann.so.2.4.9
rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4.9
rtpreceiver: CMakeFiles/rtpreceiver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fangyuan/workspace/github/algorithm/androidReceiver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable rtpreceiver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rtpreceiver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/rtpreceiver.dir/build: rtpreceiver

.PHONY : CMakeFiles/rtpreceiver.dir/build

# Object files for target rtpreceiver
rtpreceiver_OBJECTS = \
"CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o"

# External object files for target rtpreceiver
rtpreceiver_EXTERNAL_OBJECTS =

CMakeFiles/CMakeRelink.dir/rtpreceiver: CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o
CMakeFiles/CMakeRelink.dir/rtpreceiver: CMakeFiles/rtpreceiver.dir/build.make
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_videostab.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_ts.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_superres.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_ocl.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_gpu.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_contrib.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: rtph264depay/librtph264depay.a
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_photo.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_legacy.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_video.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_ml.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_flann.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4.9
CMakeFiles/CMakeRelink.dir/rtpreceiver: CMakeFiles/rtpreceiver.dir/relink.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fangyuan/workspace/github/algorithm/androidReceiver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable CMakeFiles/CMakeRelink.dir/rtpreceiver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rtpreceiver.dir/relink.txt --verbose=$(VERBOSE)

# Rule to relink during preinstall.
CMakeFiles/rtpreceiver.dir/preinstall: CMakeFiles/CMakeRelink.dir/rtpreceiver

.PHONY : CMakeFiles/rtpreceiver.dir/preinstall

CMakeFiles/rtpreceiver.dir/requires: CMakeFiles/rtpreceiver.dir/rtpreceiver.cpp.o.requires

.PHONY : CMakeFiles/rtpreceiver.dir/requires

CMakeFiles/rtpreceiver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/rtpreceiver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/rtpreceiver.dir/clean

CMakeFiles/rtpreceiver.dir/depend:
	cd /home/fangyuan/workspace/github/algorithm/androidReceiver && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fangyuan/workspace/github/algorithm/androidReceiver /home/fangyuan/workspace/github/algorithm/androidReceiver /home/fangyuan/workspace/github/algorithm/androidReceiver /home/fangyuan/workspace/github/algorithm/androidReceiver /home/fangyuan/workspace/github/algorithm/androidReceiver/CMakeFiles/rtpreceiver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/rtpreceiver.dir/depend

