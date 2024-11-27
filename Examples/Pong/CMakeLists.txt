cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Examples/Pong)

add_executable(Pong
        ${SHARED}/Panic.hpp
        Source/Pong.cpp
)

target_link_libraries(Pong PRIVATE
        XenEngine
        lua
        sol2
        pugixml::pugixml
        glfw
)