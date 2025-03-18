cmake_minimum_required(VERSION 3.12)

project(basic-interaction-example)

set(IMGUI_NODE_EDITOR_ROOT_DIR "${CMAKE_SOURCE_DIR}/libs/imgui-node-editor" CACHE PATH "Path to ImGui Node Editor")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

list(APPEND CMAKE_MODULE_PATH "${IMGUI_NODE_EDITOR_ROOT_DIR}/misc/cmake-modules")

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/libs/SDL2")


find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)
find_package(SDL2 REQUIRED)

add_subdirectory(libs/imgui-node-editor/examples/application)
add_subdirectory(libs/json)

add_executable(basic-interaction-example 
    ${CMAKE_SOURCE_DIR}/basic-example.cpp
    ${CMAKE_SOURCE_DIR}/AudioOutput.cpp
    ${CMAKE_SOURCE_DIR}/Oscillator.cpp
    ${CMAKE_SOURCE_DIR}/Oscilloscope.cpp  
    ${CMAKE_SOURCE_DIR}/AudioModule.cpp
    ${CMAKE_SOURCE_DIR}/Adder.cpp
    ${CMAKE_SOURCE_DIR}/Reverb.cpp
    ${CMAKE_SOURCE_DIR}/Distortion.cpp
    ${CMAKE_SOURCE_DIR}/NoiseGenerator.cpp
    ${CMAKE_SOURCE_DIR}/Control.cpp
)

target_link_libraries(basic-interaction-example PRIVATE imgui imgui_node_editor SDL2::SDL2 application nlohmann_json::nlohmann_json)

add_custom_command(TARGET basic-interaction-example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/libs/imgui-node-editor/examples/data"
        $<TARGET_FILE_DIR:basic-interaction-example>/data
)

set_target_properties(basic-interaction-example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)