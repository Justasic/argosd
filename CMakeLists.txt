cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# set a default build type if none was specified
set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
		STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

# Always use color diagnostics
set (CMAKE_COLOR_DIAGNOSTICS TRUE)
# Always export compile commands
set (CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Define the C project
project(argosd
	VERSION 1.0
	DESCRIPTION "Adaptive Resource Governor for Operating Systems"
	LANGUAGES C
)

# Collect C source files from various subdirectories
set(CXX_SOURCES)
file(GLOB_RECURSE C_SOURCES ${CMAKE_CURRENT_SOURCE_DIR} "src/*.c")
list(SORT C_SOURCES)

# Tell CMake these are C sources
set_source_files_properties(${C_SOURCES} 
	PROPERTIES 
		LANGUAGE C
)

# Generate a configure file.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/sysconf.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/sysconf.h)

# Create a new target executable
add_executable(${PROJECT_NAME} ${C_SOURCES})

# Specify compile definitions
target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		_GNU_SOURCE=1
)

# Tell CMake we want to use C11 and not to include language extensions
set_target_properties(${PROJECT_NAME}
	PROPERTIES
		LINKER_LANGUAGE C
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)

# Add include directories
target_include_directories(${PROJECT_NAME}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
)

# We link against android's liblog for logging facilities.
target_link_libraries(${PROJECT_NAME}
	PUBLIC 
		log
)
