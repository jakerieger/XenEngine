cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools/XBuild)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

add_executable(XBuild
        ${VEND}/sha256.h
        ${VEND}/sha256.cpp
        ${SHARED}/Compression.hpp
        Source/main.cpp
)

find_package(CLI11 CONFIG REQUIRED)

target_link_libraries(XBuild PRIVATE
        XenEngine
        pugixml::pugixml
        CLI11::CLI11
        liblzma::liblzma
        ZLIB::ZLIB
)