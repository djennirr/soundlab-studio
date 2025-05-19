cmake_minimum_required(VERSION 3.15)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем CMake-модули ImGui Node Editor
set(IMGUI_NODE_EDITOR_ROOT_DIR "${CMAKE_SOURCE_DIR}/libs/imgui-node-editor" CACHE PATH "Path to ImGui Node Editor")
list(APPEND CMAKE_MODULE_PATH "${IMGUI_NODE_EDITOR_ROOT_DIR}/misc/cmake-modules")

# Поиск локальных библиотек
find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)

# SDL2 — из Conan
find_package(SDL2 REQUIRED CONFIG)

# Подключаем локальные библиотеки
add_subdirectory(libs/imgui-node-editor/examples/application)
add_subdirectory(libs/json)

# Основной исполняемый файл
add_executable(main
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/themes/themes.cpp
    ${CMAKE_SOURCE_DIR}/src/AudioOutput.cpp
    ${CMAKE_SOURCE_DIR}/src/Oscillator.cpp
    ${CMAKE_SOURCE_DIR}/src/Oscilloscope.cpp  
    ${CMAKE_SOURCE_DIR}/src/Module.cpp
    ${CMAKE_SOURCE_DIR}/src/Adder.cpp
    ${CMAKE_SOURCE_DIR}/src/Reverb.cpp
    ${CMAKE_SOURCE_DIR}/src/Distortion.cpp
    ${CMAKE_SOURCE_DIR}/src/NoiseGenerator.cpp
    ${CMAKE_SOURCE_DIR}/src/Sampler.cpp
    ${CMAKE_SOURCE_DIR}/src/Control.cpp
    ${CMAKE_SOURCE_DIR}/src/ADSR.cpp
    ${CMAKE_SOURCE_DIR}/src/Filter.cpp
    ${CMAKE_SOURCE_DIR}/src/Sequencer.cpp
)

# Линкуем библиотеки
target_link_libraries(main PRIVATE
    imgui
    imgui_node_editor
    SDL2::SDL2
    application
    nlohmann_json::nlohmann_json
)

# Копирование ресурсов
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/libs/imgui-node-editor/examples/data"
        $<TARGET_FILE_DIR:main>/data
)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/samples"
        $<TARGET_FILE_DIR:main>/samples
)

# Папка вывода
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)