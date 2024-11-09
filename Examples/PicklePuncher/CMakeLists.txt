cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Examples/PicklePuncher)

add_executable(PicklePuncher
        ${SHARED}/Panic.hpp
        PicklePuncher.cpp
)

target_link_libraries(PicklePuncher PRIVATE
        XenEngine
)

# This command will only succeed if XPak has been ran prior!
add_custom_command(TARGET PicklePuncher PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Content/Build ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Content)