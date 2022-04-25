# This file contains sets of common compiler flags bundled together into CMake targets.
# The flags are intended to be used as nice defaults for almost every project.
# To use the flags link to one of the targets using `target_link_libraries`.
# C and C++ projects are supported.
# WARNING: Putting warning flags into INTERFACE properties of libraries is ill-advised as it
# may lead to redundant noisy warning messages for the clients. Using ABI-affecting flags that are
# inconsistent across the dependency tree is going to cause broken builds and silent runtime errors.

if(${CMAKE_CXX_COMPILER_ID})
	set(compiler_id ${CMAKE_CXX_COMPILER_ID})
	set(simulate_id ${CMAKE_CXX_SIMULATE_ID})
else()
	set(compiler_id ${CMAKE_C_COMPILER_ID})
	set(simulate_id ${CMAKE_C_SIMULATE_ID})
endif()

# Warnings
set(msvc_warnings
	# Baseline reasonable warnings
	/W4
	# 'identifier': conversion from 'type1' to 'type1', possible loss of data
	/w14242
	# 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible
	# loss of data
	/w14254
	# 'function': member function does not override any base class virtual member function
	/w14263
	# 'classname': class has virtual functions, but destructor is not virtual. Instances of
	# this class may not be destructed correctly
	/w14265
	# 'operator': unsigned/negative constant mismatch
	/w14287
	# nonstandard extension used: 'variable': loop control variable declared in the
	# for-loop is used outside the for-loop scope
	/we4289
	# 'operator': expression is always 'boolean_value'
	/w14296
	# 'variable': pointer truncation from 'type1' to 'type2'
	/w14311
	# expression before comma evaluates to a function which is missing an argument list
	/w14545
	# function call before comma missing argument list
	/w14546
	# 'operator': operator before comma has no effect; expected operator with side-effect
	/w14547
	# 'operator': operator before comma has no effect; did you intend 'operator'?
	/w14549
	# expression has no effect; expected expression with side- effect
	/w14555
	# pragma warning: there is no warning number 'number'
	/w14619
	# Enable warning on thread unsafe static member initialization
	/w14640
	# Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected
	# runtime behavior
	/w14826
	# wide string literal cast to 'LPSTR'
	/w14905
	# string literal cast to 'LPWSTR'
	/w14906
	# Ullegal copy-initialization; more than one user-defined conversion has been implicitly applied
	/w14928
	# Standards conformance mode for MSVC compiler
	/permissive-
)

set(clang_gcc_warnings
	# Not *all* actually, just uncontroversial ones
	-Wall
	# Reasonable and standard
	-Wextra
	# Warn the user if a variable declaration shadows one from a parent context
	-Wshadow
	# C++ only. Warn the user if a class with virtual functions has a non-virtual destructor
	$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-Wnon-virtual-dtor>
	# C++ only. Warn on trying to call a non-virtual destructor on an abstract class
	$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-Wdelete-non-virtual-dtor>
	# C++ only. Warn for c-style casts
	$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-Wold-style-cast>
	# Warn whenever a pointer is cast such that the required alignment of the target is increased
	-Wcast-align
	# Warn on anything being unused
	-Wunused
	# C++ only. Warn if you overload (not override) a virtual function
	$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-Woverloaded-virtual>
	# Warn if non-standard C or C++ is used
	-Wpedantic
	# Warn on type conversions that may lose data
	-Wconversion
	# Warn on sign conversions
	-Wsign-conversion
	# Warn if a null dereference is detected
	-Wnull-dereference
	# Warn if float is implicitly promoted to double
	-Wdouble-promotion
	# Warn on security issues around functions that format output (ie printf)
	-Wformat=2
	# Warn if a global function is defined without a previous prototype declaration
	$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,C>:-Wmissing-prototypes>
	# Warn if a function is declared or defined without specifying the argument types
	$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,C>:-Wstrict-prototypes>
)

set(clang_warnings
	${clang_gcc_warnings}
	# Warn about code that can't be executed
	-Wunreachable-code
	# Warn when a switch case falls through
	-Wimplicit-fallthrough
)

set(gcc_warnings
	${clang_gcc_warnings}
	# Warn if indentation implies blocks where blocks do not exist
	-Wmisleading-indentation
	# Warn if if / else chain has duplicated conditions
	-Wduplicated-cond
	# Warn if if / else branches have duplicated code
	-Wduplicated-branches
	# Warn about logical operations being used where bitwise were probably wanted
	-Wlogical-op
	# Warn if you perform a cast to the same type. Disabled for now
#	$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-Wuseless-cast>
	# Warn if a goto statement or a switch statement jumps forward across the initialization of
	# a variable
	$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,C>:-Wjump-misses-init>
	# Warn if a user-supplied include directory does not exist
	-Wmissing-include-dirs
	# Warn if a precompiled header is found in the search path but cannot be used
	-Winvalid-pch
	# Warn when a switch case falls through. Do not recognize FALLTHRU comments
	-Wimplicit-fallthrough=5
)

if(MSVC)
	set(project_warnings ${msvc_warnings})
elseif(compiler_id MATCHES ".*Clang" AND NOT simulate_id STREQUAL "MSVC")
	set(project_warnings ${clang_warnings})
elseif(compiler_id STREQUAL "GNU")
	set(project_warnings ${gcc_warnings})
else()
	message(AUTHOR_WARNING "No compiler warnings set for '${CMAKE_C_COMPILER_ID}' compiler.")
endif()

add_library(cflags_default_warnings INTERFACE)
add_library(cflags::default_warnings ALIAS cflags_default_warnings)
target_compile_options(cflags_default_warnings INTERFACE ${project_warnings})

# Errors
set(msvc_errors
	# Error on missing return in a non-void function. Enabled by Microsoft by default
	/we4716
)

set(clang_gcc_errors
	# Error on missing return in a non-void function
	-Werror=return-type
	# Error whenever a switch lacks a case for a named code of the enumeration
#	-Werror=switch
	# Error when a switch case falls through. Use [[fallthrough]] to fix
#	-Werror=implicit-fallthrough
	# Error on implicit function declaration
	-Werror=implicit-function-declaration
)

set(clang_errors ${clang_gcc_errors})
set(gcc_errors ${clang_gcc_errors})

if(MSVC)
	set(project_errors ${msvc_errors})
elseif(compiler_id MATCHES ".*Clang" AND NOT simulate_id STREQUAL "MSVC")
	set(project_errors ${clang_errors})
elseif(compiler_id STREQUAL "GNU")
	set(project_errors ${gcc_errors})
else()
	message(AUTHOR_WARNING "No compiler errors set for '${compiler_id}' compiler.")
endif()

add_library(cflags_default_errors INTERFACE)
add_library(cflags::default_errors ALIAS cflags_default_errors)
target_compile_options(cflags_default_errors INTERFACE ${project_errors})

# Compile options
add_library(cflags_default_compile_options INTERFACE)
add_library(cflags::default_compile_options ALIAS cflags_default_compile_options)

set(msvc_defines
	# Do not warn on "unsafe" C functions
	_CRT_SECURE_NO_WARNINGS
	# Remove min/max macros from Windef.h (and Windows.h)
	NOMINMAX
	# Do not include unnecessary WinAPI headers
	WIN32_LEAN_AND_MEAN
)

set(clang_gcc_compile_options
	-fno-omit-frame-pointer
	-fvisibility=hidden
	$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-fvisibility-inlines-hidden>
)

if(MSVC)
	target_compile_definitions(cflags_default_compile_options INTERFACE ${msvc_defines})
elseif(compiler_id STREQUAL "GNU" OR (compiler_id MATCHES ".*Clang" AND NOT simulate_id STREQUAL "MSVC"))
	target_compile_options(cflags_default_compile_options INTERFACE ${clang_gcc_compile_options})
endif()
