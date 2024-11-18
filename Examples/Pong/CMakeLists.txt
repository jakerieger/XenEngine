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

# This command will only succeed if XPak has been ran prior!
add_custom_command(TARGET Pong PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Content/Build ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Content)

add_custom_command(TARGET Pong PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Scripts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Scripts)

add_custom_command(TARGET Pong PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Scenes ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Scenes)