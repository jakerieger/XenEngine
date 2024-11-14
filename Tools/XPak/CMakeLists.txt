cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools/XPak)

add_executable(XPak
        Source/main.cpp
)

find_package(CLI11 CONFIG REQUIRED)

target_link_libraries(XPak PRIVATE
        XenEngine
        pugixml::pugixml
        CLI11::CLI11
)