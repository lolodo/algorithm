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
include rtph264depay/CMakeFiles/rtph264depay.dir/depend.make

# Include the progress variables for this target.
include rtph264depay/CMakeFiles/rtph264depay.dir/progress.make

# Include the compile flags for this target's objects.
include rtph264depay/CMakeFiles/rtph264depay.dir/flags.make

rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o: rtph264depay/CMakeFiles/rtph264depay.dir/flags.make
rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o: rtph264depay/RtpH264Depay.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fangyuan/workspace/github/algorithm/androidReceiver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o"
	cd /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o -c /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay/RtpH264Depay.cpp

rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.i"
	cd /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay/RtpH264Depay.cpp > CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.i

rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.s"
	cd /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay/RtpH264Depay.cpp -o CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.s

rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o.requires:

.PHONY : rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o.requires

rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o.provides: rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o.requires
	$(MAKE) -f rtph264depay/CMakeFiles/rtph264depay.dir/build.make rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o.provides.build
.PHONY : rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o.provides

rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o.provides.build: rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o


# Object files for target rtph264depay
rtph264depay_OBJECTS = \
"CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o"

# External object files for target rtph264depay
rtph264depay_EXTERNAL_OBJECTS =

rtph264depay/librtph264depay.a: rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o
rtph264depay/librtph264depay.a: rtph264depay/CMakeFiles/rtph264depay.dir/build.make
rtph264depay/librtph264depay.a: rtph264depay/CMakeFiles/rtph264depay.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fangyuan/workspace/github/algorithm/androidReceiver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library librtph264depay.a"
	cd /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay && $(CMAKE_COMMAND) -P CMakeFiles/rtph264depay.dir/cmake_clean_target.cmake
	cd /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rtph264depay.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
rtph264depay/CMakeFiles/rtph264depay.dir/build: rtph264depay/librtph264depay.a

.PHONY : rtph264depay/CMakeFiles/rtph264depay.dir/build

rtph264depay/CMakeFiles/rtph264depay.dir/requires: rtph264depay/CMakeFiles/rtph264depay.dir/RtpH264Depay.cpp.o.requires

.PHONY : rtph264depay/CMakeFiles/rtph264depay.dir/requires

rtph264depay/CMakeFiles/rtph264depay.dir/clean:
	cd /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay && $(CMAKE_COMMAND) -P CMakeFiles/rtph264depay.dir/cmake_clean.cmake
.PHONY : rtph264depay/CMakeFiles/rtph264depay.dir/clean

rtph264depay/CMakeFiles/rtph264depay.dir/depend:
	cd /home/fangyuan/workspace/github/algorithm/androidReceiver && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fangyuan/workspace/github/algorithm/androidReceiver /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay /home/fangyuan/workspace/github/algorithm/androidReceiver /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay /home/fangyuan/workspace/github/algorithm/androidReceiver/rtph264depay/CMakeFiles/rtph264depay.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : rtph264depay/CMakeFiles/rtph264depay.dir/depend
