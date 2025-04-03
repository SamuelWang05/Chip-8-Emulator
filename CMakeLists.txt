cmake_minimum_required(VERSION 3.30)
project(Chip_8_Interpreter)
find_package(Threads)
find_package(Git QUIET)

# Set compiler flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pedantic -pedantic-errors -g")

# For SDL3
set(SDL3_INCLUDE_DIR C:/libs/SDL/include)
set(SDL3_LIB_DIR C:/libs/SDL/lib/x86)

include_directories(${SDL3_INCLUDE_DIR})
link_directories(${SDL3_LIB_DIR})

add_executable(Chip_8_Interpreter main.cpp
        src/chip8.h
        src/chip8.cpp)

# Include project headers
include_directories(src)
# Define the source files and dependencies for the executable
set(SOURCE_FILES
        main.cpp
)

# Make the project root directory the working directory when we run
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)