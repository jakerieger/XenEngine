cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools/XPak)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

add_executable(XPak
        Source/Asset.hpp
        Source/Manifest.hpp
        Source/BuildCache.hpp
        Source/Processors.inl
        Source/main.cpp
)

find_package(OpenSSL REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

target_link_libraries(XPak PRIVATE
        XenEngine
        pugixml::pugixml
        CLI11::CLI11
        OpenSSL::SSL
        OpenSSL::Crypto
)