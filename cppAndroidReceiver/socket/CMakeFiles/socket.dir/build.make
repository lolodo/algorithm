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
CMAKE_SOURCE_DIR = /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver

# Include any dependencies generated for this target.
include socket/CMakeFiles/socket.dir/depend.make

# Include the progress variables for this target.
include socket/CMakeFiles/socket.dir/progress.make

# Include the compile flags for this target's objects.
include socket/CMakeFiles/socket.dir/flags.make

socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o: socket/CMakeFiles/socket.dir/flags.make
socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o: socket/PracticalSocket.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o"
	cd /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/socket.dir/PracticalSocket.cpp.o -c /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket/PracticalSocket.cpp

socket/CMakeFiles/socket.dir/PracticalSocket.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/socket.dir/PracticalSocket.cpp.i"
	cd /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket/PracticalSocket.cpp > CMakeFiles/socket.dir/PracticalSocket.cpp.i

socket/CMakeFiles/socket.dir/PracticalSocket.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/socket.dir/PracticalSocket.cpp.s"
	cd /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket/PracticalSocket.cpp -o CMakeFiles/socket.dir/PracticalSocket.cpp.s

socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o.requires:

.PHONY : socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o.requires

socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o.provides: socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o.requires
	$(MAKE) -f socket/CMakeFiles/socket.dir/build.make socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o.provides.build
.PHONY : socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o.provides

socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o.provides.build: socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o


# Object files for target socket
socket_OBJECTS = \
"CMakeFiles/socket.dir/PracticalSocket.cpp.o"

# External object files for target socket
socket_EXTERNAL_OBJECTS =

socket/libsocket.a: socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o
socket/libsocket.a: socket/CMakeFiles/socket.dir/build.make
socket/libsocket.a: socket/CMakeFiles/socket.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libsocket.a"
	cd /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket && $(CMAKE_COMMAND) -P CMakeFiles/socket.dir/cmake_clean_target.cmake
	cd /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/socket.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
socket/CMakeFiles/socket.dir/build: socket/libsocket.a

.PHONY : socket/CMakeFiles/socket.dir/build

socket/CMakeFiles/socket.dir/requires: socket/CMakeFiles/socket.dir/PracticalSocket.cpp.o.requires

.PHONY : socket/CMakeFiles/socket.dir/requires

socket/CMakeFiles/socket.dir/clean:
	cd /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket && $(CMAKE_COMMAND) -P CMakeFiles/socket.dir/cmake_clean.cmake
.PHONY : socket/CMakeFiles/socket.dir/clean

socket/CMakeFiles/socket.dir/depend:
	cd /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket /home/fangyuan/workspace/github/algorithm/cppAndroidReceiver/socket/CMakeFiles/socket.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : socket/CMakeFiles/socket.dir/depend

