cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools/XPak)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

add_executable(XPak
        ${STB_SRCS}
        ${VEND}/sha256.h
        ${VEND}/sha256.cpp
        ${SHARED}/Compression.hpp
        Source/Asset.hpp
        Source/Manifest.hpp
        Source/BuildCache.hpp
        Source/Processors.inl
        Source/main.cpp
        Source/PakFile.hpp
        Source/MetadataFile.hpp
)

find_package(CLI11 CONFIG REQUIRED)

target_link_libraries(XPak PRIVATE
        XenEngine
        pugixml::pugixml
        CLI11::CLI11
        liblzma::liblzma
)