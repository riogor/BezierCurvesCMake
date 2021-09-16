cmake_minimum_required(VERSION 3.0.0)
project(BezierCurves)

find_package(OpenGL REQUIRED)
find_package(freeglut QUIET)

IF(NOT freeglut_FOUND)
	add_subdirectory(submodules/freeglut)
ENDIF()

set(CMAKE_BUILD_TYPE Release)

set(SOURCES ${SOURCES} main.cpp)
add_executable(Bezier WIN32 ${SOURCES})

IF(NOT freeglut_FOUND)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/submodules/freeglut/include/GL)
	target_link_libraries(Bezier ${OPENGL_LIBRARIES} freeglut)
ENDIF()