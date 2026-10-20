# 1. Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.15)

# 2. Name your project and state the language
project(MyCPlusPlusProject VERSION 1.0 LANGUAGES CXX)

# 3. Enforce C++17 or C++20 standard globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Tell CMake where to look for header files (#include "...")
include_directories(src)

# 5. Define your final executable and list all source files it needs
add_executable(my_program 
    src/Source.cpp 
)