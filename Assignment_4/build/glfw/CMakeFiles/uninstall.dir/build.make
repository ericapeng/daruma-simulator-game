# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/ericapeng/Documents/Computer Graphics/Assignments/Assignment4/Assignment_4"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/ericapeng/Documents/Computer Graphics/Assignments/Assignment4/Assignment_4/build"

# Utility rule file for uninstall.

# Include the progress variables for this target.
include glfw/CMakeFiles/uninstall.dir/progress.make

glfw/CMakeFiles/uninstall:
	cd "/Users/ericapeng/Documents/Computer Graphics/Assignments/Assignment4/Assignment_4/build/glfw" && /Applications/CMake.app/Contents/bin/cmake -P /Users/ericapeng/Documents/Computer\ Graphics/Assignments/Assignment4/Assignment_4/build/glfw/cmake_uninstall.cmake

uninstall: glfw/CMakeFiles/uninstall
uninstall: glfw/CMakeFiles/uninstall.dir/build.make

.PHONY : uninstall

# Rule to build all files generated by this target.
glfw/CMakeFiles/uninstall.dir/build: uninstall

.PHONY : glfw/CMakeFiles/uninstall.dir/build

glfw/CMakeFiles/uninstall.dir/clean:
	cd "/Users/ericapeng/Documents/Computer Graphics/Assignments/Assignment4/Assignment_4/build/glfw" && $(CMAKE_COMMAND) -P CMakeFiles/uninstall.dir/cmake_clean.cmake
.PHONY : glfw/CMakeFiles/uninstall.dir/clean

glfw/CMakeFiles/uninstall.dir/depend:
	cd "/Users/ericapeng/Documents/Computer Graphics/Assignments/Assignment4/Assignment_4/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/ericapeng/Documents/Computer Graphics/Assignments/Assignment4/Assignment_4" "/Users/ericapeng/Documents/Computer Graphics/Assignments/Assignment4/ext/glfw" "/Users/ericapeng/Documents/Computer Graphics/Assignments/Assignment4/Assignment_4/build" "/Users/ericapeng/Documents/Computer Graphics/Assignments/Assignment4/Assignment_4/build/glfw" "/Users/ericapeng/Documents/Computer Graphics/Assignments/Assignment4/Assignment_4/build/glfw/CMakeFiles/uninstall.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : glfw/CMakeFiles/uninstall.dir/depend
