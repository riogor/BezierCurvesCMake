cmake_minimum_required(VERSION 3.21)
project(BezierCurves)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} C:/MinGW/include/freeglut)

set(SOURCES main.cpp)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
#add_compile_options(-lfreeglut -lopengl32 -lglu32 -Wl,--subsystem,windows)
#add_compile_options(-g)

add_executable(Bezier ${SOURCES})

target_link_libraries(Bezier ${OPENGL_LIBRARIES} C:/MinGW/lib/freeglut/freeglut.lib)