cmake_minimum_required(VERSION 3.21) # At least CMake v3.21 is needed because of C17 support

project(cpp_code_samples
	DESCRIPTION "A collection of code samples and examples for projects in C++"
	LANGUAGES C CXX
)

# Tell the compiler to run in C++20 mode (ISO/IEC 14882:2020).
set(CMAKE_CXX_STANDARD 20)
# This should be set to TRUE to fail if the compiler doesn't support C++20
set(CMAKE_CXX_STANDARD_REQUIRED FALSE)
# Turn the compiler extensions off (i.e. it forces -std=c++20 instead of -std=gnu++20 on gcc and clang)
set(CMAKE_CXX_EXTENSIONS FALSE)

# Use folders for nice tree in Visual Studio and XCode
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

# Generate compile_commands.json which is a format widely accepted by third-party tools
# (code formatters, static analyzers, linters, project viewers, LSP servers, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Under Linux, the mathematical functions from <math.h> are defined in a separate library libm.
# You can link to this target if you use <math.h>
add_library(stdlib_math INTERFACE)
add_library(stdlib::math ALIAS stdlib_math)
if(UNIX)
	target_link_libraries(stdlib_math INTERFACE m)
endif()

include(cmake/CompilerFlags.cmake)

# Define a single target with unified compiler flags
add_library(flags INTERFACE)
add_library(flags::flags ALIAS flags)

target_link_libraries(flags INTERFACE
	cflags::default_warnings
	cflags::default_errors
	cflags::default_compile_options
)
# Enable include paths relative to the `src` directory
target_include_directories(flags INTERFACE src)

add_executable(hello_world src/hello_world/hello_world_main.cpp)
target_link_libraries(hello_world PRIVATE flags::flags)

add_executable(bitset
	src/bitset/bitset.hpp
	src/bitset/bitset.cpp
	src/bitset/bitset_main.cpp
)
target_link_libraries(bitset PRIVATE flags::flags)
add_executable(expression_tree
	src/expression_tree/expression.hpp
	src/expression_tree/operators.hpp
	src/expression_tree/functions.hpp
	src/expression_tree/expression_tree_main.cpp
)
target_link_libraries(expression_tree PRIVATE flags::flags stdlib::math)

add_executable(array_view
	src/array_view/array_view.hpp
	src/array_view/array_view_main.cpp
)
target_link_libraries(array_view PRIVATE flags::flags)

add_executable(scoped_ptr
	src/scoped_ptr/scoped_ptr.hpp
	src/scoped_ptr/scoped_ptr_main.cpp
)
target_link_libraries(scoped_ptr PRIVATE flags::flags)

add_executable(list
	src/list/list.hpp
	src/list/list_main.cpp
)
target_link_libraries(list PRIVATE flags::flags)
