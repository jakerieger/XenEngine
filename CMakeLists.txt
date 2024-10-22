cmake_minimum_required(VERSION 3.29)
project(XenEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
set(INC ${CMAKE_CURRENT_SOURCE_DIR}/Include/XenEngine)
set(VEND ${CMAKE_CURRENT_SOURCE_DIR}/Vendor)
set(TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/Tools)

include_directories(
        ${SRC}
        ${INC}
        ${VEND}
)

add_library(XenEngine STATIC
        ${INC}/Platform.h
        ${INC}/XenEngine.h
        ${SRC}/XenEngine.cpp
)

target_link_libraries(XenEngine PRIVATE
        dxgi.lib
        d2d1.lib
        d3d11.lib
        winmm.lib
)

add_executable(XnSimpleGame Examples/SimpleGame.cpp)
target_link_libraries(XnSimpleGame PRIVATE XenEngine)
