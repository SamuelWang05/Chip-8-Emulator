cmake_minimum_required(VERSION 3.30)
project(Chip_8_Interpreter)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -pedantic -pedantic-errors -g)

# SDL2 include and lib paths (adjust these to match your system if needed)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "C:/libs/SDL2/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

# Add source files
add_executable(Chip_8_Interpreter
        main.cpp
        src/chip8.cpp
        src/chip8.h
)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})