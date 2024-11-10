cmake_minimum_required(VERSION 3.14)
project(XenEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
set(INC ${CMAKE_CURRENT_SOURCE_DIR}/Include/XenEngine)
set(SHARED ${CMAKE_CURRENT_SOURCE_DIR}/Shared)
set(VEND ${CMAKE_CURRENT_SOURCE_DIR}/Vendor)
set(TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/Tools)

set(GLAD_SRCS
        ${VEND}/glad.c
        ${VEND}/glad/glad.h
)

file(GLOB IMGUI_SRCS
        ${VEND}/imgui/*.h
        ${VEND}/imgui/*.cpp
        ${VEND}/imgui/backends/imgui_impl_glfw*
        ${VEND}/imgui/backends/imgui_impl_opengl3*
)

include_directories(
        ${SRC}
        ${INC}
        ${SHARED}
        ${VEND}
        ${VEND}/imgui
        ${VEND}/imgui/backends
)

add_library(XenEngine STATIC
        ${SHARED}/Panic.hpp
        ${INC}/Platform.hpp
        ${INC}/XenEngine.hpp
        ${SRC}/XenEngine.cpp
)

find_package(sol2 CONFIG REQUIRED)
find_package(lua CONFIG REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(GLFW3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(XenEngine PRIVATE
        lua
        sol2
        pugixml::shared
        glfw
        glm::glm
)

# TOOLS #
add_subdirectory(Tools/XEditor)

add_subdirectory(Examples/PicklePuncher)