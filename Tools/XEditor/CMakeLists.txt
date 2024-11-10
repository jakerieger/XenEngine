cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools/XEditor)

add_executable(XEditor
        ${SHARED}/Panic.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/main.cpp
)

target_link_libraries(XEditor PRIVATE
        XenEngine
        lua
        sol2
        pugixml::shared
)