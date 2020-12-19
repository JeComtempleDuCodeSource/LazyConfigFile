cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(LazyConfig VERSION 1.0.0)

# Build libs
add_library(LazyConfig SHARED
                "${CMAKE_CURRENT_LIST_DIR}/LazyConfigFile.cpp"
)   

# Includes
target_include_directories(LazyConfig PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

# Flags 
target_compile_options(LazyConfig PUBLIC "-Wall" "-O2") 

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)