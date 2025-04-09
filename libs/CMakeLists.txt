cmake_minimum_required(VERSION 3.15)
project(MyImGuiProject)

# Настройка стандартов C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем поддиректории с библиотеками
add_subdirectory(imgui)
add_subdirectory(ImGuiFileDialog)

# Основной исполняемый файл
add_executable(my_app main.cpp)

# Связываем зависимости
target_link_libraries(my_app PRIVATE
    imgui
    ImGuiFileDialog
)

# Настройка путей для заголовочных файлов
target_include_directories(my_app PRIVATE
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/ImGuiFileDialog
)