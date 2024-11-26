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

set(STB_SRCS
        ${VEND}/stb_image.h
        ${VEND}/stb_image.cpp
)

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
        ${INC}
        ${SHARED}
        ${SRC}
        ${VEND}
        ${VEND}/imgui
        ${VEND}/imgui/backends
)

add_library(XenEngine STATIC
        ${STB_SRCS}
        ${GLAD_SRCS}
        ${SHARED}/Compression.hpp
        ${SHARED}/Expect.hpp
        ${SHARED}/IO.hpp
        ${SHARED}/Panic.hpp
        ${SHARED}/Types.hpp
        ${INC}/Buffer.hpp
        ${INC}/Camera.hpp
        ${INC}/Clock.hpp
        ${INC}/Component.hpp
        ${INC}/CommonShaders.hpp
        ${INC}/ContentManager.hpp
        ${INC}/Game.hpp
        ${INC}/GameObject.hpp
        ${INC}/Graphics.hpp
        ${INC}/Primitives.hpp
        ${INC}/Scene.hpp
        ${INC}/SceneManager.hpp
        ${INC}/ScriptEngine.hpp
        ${INC}/Shader.hpp
        ${INC}/Texture.hpp
        ${INC}/VertexArray.hpp
        ${SRC}/Camera.cpp
        ${SRC}/Clock.cpp
        ${SRC}/ContentManager.cpp
        ${SRC}/Game.cpp
        ${SRC}/GameObject.cpp
        ${SRC}/Scene.cpp
        ${SRC}/Shader.cpp
)

find_package(sol2 CONFIG REQUIRED)
find_package(lua CONFIG REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(tinyfiledialogs CONFIG REQUIRED)
find_package(liblzma CONFIG REQUIRED)

target_link_libraries(XenEngine PRIVATE
        glfw
        glm::glm
        liblzma::liblzma
        lua
        pugixml::pugixml
        sol2
)

# TOOLS #
add_subdirectory(Tools/XEditor)
add_subdirectory(Tools/XPak)

add_subdirectory(Examples/Pong)
