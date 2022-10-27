cmake_minimum_required(VERSION 3.6.2)

##################################
# Build configuration
set(PRJ_NAME cs350_octrees)
set(PRJ_TEST_NAME ${PRJ_NAME}_test)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Source files
set(SRC
		src/debug.hpp
		src/debug.cpp
		src/geometry.cpp
		src/geometry.hpp
		src/math.hpp
		src/math.cpp
		src/pch.hpp
		src/fwd.hpp
		src/opengl.hpp
		src/opengl.cpp
		src/window.hpp
		src/window.cpp
		src/camera.hpp
		src/camera.cpp
		src/renderer.cpp
		src/renderer.hpp
		src/shader.hpp
		src/shader.cpp
		src/file.hpp
		src/file.cpp
		src/mesh.hpp
		src/mesh.cpp
		src/mesh_data.hpp
		src/mesh_data.cpp
		src/imgui.hpp
		src/bvh_tree.cpp
		src/bvh_tree.hpp
		src/octree.cpp
		src/octree.hpp)

include_directories(src)

# Test files
set(SRC_TEST
		src/test/test_octree.cpp src/test/test_common.hpp)

# Projects
project(${PRJ_NAME})
project(${PRJ_TEST_NAME})

##################################
# Libraries
if (WIN32)
	# Directory in which libraries are
	set(DEPENDENCIES_DIR $ENV{USERPROFILE}/documents/digipen/dependencies)
	string(REPLACE "\\" "/" DEPENDENCIES_DIR ${DEPENDENCIES_DIR})
elseif (UNIX)
	# Directory in which libraries are
	set(DEPENDENCIES_DIR $ENV{HOME}/digipen/dependencies)
endif ()

# glm
include_directories("${DEPENDENCIES_DIR}/glm")

# gtest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Make GTest match runtime libraries
add_subdirectory("${DEPENDENCIES_DIR}/googletest" "googletest")
enable_testing()

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${DEPENDENCIES_DIR}/glfw" "glfw")
include_directories("${DEPENDENCIES_DIR}/glfw/include/")

# glad
add_subdirectory("${DEPENDENCIES_DIR}/glad" "glad")
include_directories("${CMAKE_CACHEFILE_DIR}/glad/include/")

# lodepng
include_directories("${DEPENDENCIES_DIR}/lodepng")
set(SRC_EXTERNAL "${SRC_EXTERNAL}" "${DEPENDENCIES_DIR}/lodepng/lodepng.cpp")

# imgui
include_directories("${DEPENDENCIES_DIR}/imgui")
set(SRC_EXTERNAL "${SRC_EXTERNAL}" "${DEPENDENCIES_DIR}/imgui/examples/imgui_impl_glfw.cpp")
set(SRC_EXTERNAL "${SRC_EXTERNAL}" "${DEPENDENCIES_DIR}/imgui/examples/imgui_impl_opengl3.cpp")
set(SRC_EXTERNAL "${SRC_EXTERNAL}" "${DEPENDENCIES_DIR}/imgui/imgui.cpp")
set(SRC_EXTERNAL "${SRC_EXTERNAL}" "${DEPENDENCIES_DIR}/imgui/imgui_demo.cpp")
set(SRC_EXTERNAL "${SRC_EXTERNAL}" "${DEPENDENCIES_DIR}/imgui/imgui_draw.cpp")
set(SRC_EXTERNAL "${SRC_EXTERNAL}" "${DEPENDENCIES_DIR}/imgui/imgui_widgets.cpp")
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)

##################################
# Compile arguments
set(CMAKE_CXX_STANDARD 20)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	message("Using MSVC")
	# Visual Studio Configuration
	# Enable warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")

	# Warnings as errors
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") 
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

	# Disable specific warning
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201") # nameless struct/union
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	message("Using G++")
	# G++ configuration
	# Enable warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
	# Warnings as errors
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	# Disable specific warning
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile") # Disable GLM warning on overflow function
endif ()
 
# Binaries
add_executable(${PRJ_NAME} ${SRC} ${SRC_EXTERNAL} src/main.cpp src/demo_octree.cpp src/demo_octree.hpp)
target_link_libraries(${PRJ_NAME} glfw glad)

# Test binaries
add_executable(${PRJ_TEST_NAME} ${SRC} ${SRC_TEST} ${SRC_EXTERNAL})
include_directories(${PRJ_TEST_NAME} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(${PRJ_TEST_NAME} glfw glad gtest_main)
add_test(NAME ${PRJ_TEST_NAME}  COMMAND ${PRJ_TEST_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})