cmake_minimum_required(VERSION 3.2)

project(hw1)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(extern/angel06)

set(SOURCES src/main.cpp
            src/drone.cpp)

set(HEADERS include/drone.h
            )            

add_executable(hw1 ${SOURCES} ${HEADERS})
target_link_libraries(hw1 Angel ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})
target_include_directories(hw1 PUBLIC include ${ANGEL_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})