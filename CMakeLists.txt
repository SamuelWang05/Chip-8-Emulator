cmake_minimum_required(VERSION 3.30)
project(Chip_8_Interpreter)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pedantic -pedantic-errors -g")

# SDL3 include and lib paths (based on x86_64-w64-mingw32 layout)
set(SDL3_ROOT C:/libs/SDL3/x86_64-w64-mingw32)
set(SDL3_INCLUDE_DIR ${SDL3_ROOT}/include)
set(SDL3_LIB_DIR ${SDL3_ROOT}/lib)

add_executable(Chip_8_Interpreter main.cpp
        src/chip8.cpp
        src/chip8.h)

target_include_directories(Chip_8_Interpreter PRIVATE ${SDL3_INCLUDE_DIR})
target_link_directories(Chip_8_Interpreter PRIVATE ${SDL3_LIB_DIR})
target_link_libraries(Chip_8_Interpreter PRIVATE SDL3)

# Output binary to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)