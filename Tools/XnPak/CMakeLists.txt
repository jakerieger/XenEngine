cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools)

add_executable(XnPak
        ${SHARED}/Panic.h
        XnPak.hpp
        XnPak.cpp
        main.cpp
)

target_link_libraries(XnPak PRIVATE
        CLI11::CLI11
        pugixml::static
)

add_custom_command(TARGET XnPak PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Example ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Example)