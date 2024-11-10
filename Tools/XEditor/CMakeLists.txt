cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools/XEditor)

add_executable(XEditor
        ${GLAD_SRCS}
        ${SHARED}/Panic.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/main.cpp
        Source/EditorWindow.cpp
        Source/EditorWindow.hpp
        Source/EditorUI.cpp
        Source/EditorUI.hpp
)

target_link_libraries(XEditor PRIVATE
        XenEngine
        lua
        sol2
        pugixml::shared
        glfw
        glm::glm
)