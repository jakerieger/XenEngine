cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools/XEditor)

add_executable(XEditor
        ${GLAD_SRCS}
        ${IMGUI_SRCS}
        ${SHARED}/Panic.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/main.cpp
        Source/EditorWindow.cpp
        Source/EditorWindow.hpp
        Source/EditorUI.cpp
        Source/EditorUI.hpp
        Source/EditorStyle.hpp
        Source/EditorStyle.cpp
        Source/EditorLog.cpp
        Source/EditorLog.h
        Source/EditorPreferences.cpp
        Source/EditorPreferences.hpp
)

target_link_libraries(XEditor PRIVATE
        XenEngine
        lua
        sol2
        pugixml::pugixml
        glfw
        glm::glm
        tinyfiledialogs::tinyfiledialogs
        liblzma::liblzma
)

add_custom_command(TARGET XEditor PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Themes ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Themes)