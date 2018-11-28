cmake_minimum_required(VERSION 3.10)
project(OpenGL_BabySteps)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(utils)

add_executable(OpenGL_BabySteps
        main.cpp
        utils)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)
find_package (OpenGL REQUIRED)

target_link_libraries(OpenGL_BabySteps ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} utils)
target_include_directories(OpenGL_BabySteps PRIVATE ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} /usr/local/include/glm)