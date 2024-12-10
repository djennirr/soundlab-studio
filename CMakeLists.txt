cmake_minimum_required(VERSION 3.12)

project(basic-interaction-example)

# Укажите путь к imgui-node-editor
set(IMGUI_NODE_EDITOR_ROOT_DIR "${CMAKE_SOURCE_DIR}/libs/imgui-node-editor" CACHE PATH "Path to ImGui Node Editor")

# Укажите стандарт C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Добавьте путь к модулям CMake из imgui-node-editor
list(APPEND CMAKE_MODULE_PATH "${IMGUI_NODE_EDITOR_ROOT_DIR}/misc/cmake-modules")

# Укажите путь к SDL2
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/libs/SDL2")

# Найдите пакеты imgui и imgui-node-editor
find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)
find_package(SDL2 REQUIRED)

# Добавьте подпроект для библиотеки application
add_subdirectory(libs/imgui-node-editor/examples/application)

# Добавьте исполняемый файл для basic-interactions.cpp
add_executable(basic-interaction-example 
    ${CMAKE_SOURCE_DIR}/basic-example.cpp
    ${CMAKE_SOURCE_DIR}/AudioOutput.cpp  # Добавьте ваш файл
    ${CMAKE_SOURCE_DIR}/Oscillator.cpp  
    ${CMAKE_SOURCE_DIR}/AudioModule.cpp
    ${CMAKE_SOURCE_DIR}/Adder.cpp
)

# Подключите библиотеки, включая SDL2, imgui и imgui_node_editor
target_link_libraries(basic-interaction-example PRIVATE imgui imgui_node_editor SDL2::SDL2 application)

add_custom_command(TARGET basic-interaction-example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/libs/imgui-node-editor/examples/data"
        $<TARGET_FILE_DIR:basic-interaction-example>/data
)

# Установите папку для вывода исполняемых файлов
set_target_properties(basic-interaction-example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)