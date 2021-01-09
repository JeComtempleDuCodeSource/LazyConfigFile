cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(LCFExample VERSION 1.0.0)

# Source files
add_executable(LCFExample
                "${CMAKE_CURRENT_LIST_DIR}/main.cpp" 
)

# Includes
target_include_directories(LCFExample PUBLIC
                          "${CMAKE_CURRENT_LIST_DIR}"
                          "${LAZYCONFIG_DIR}"
)

# Flags
target_compile_options(LCFExample PUBLIC "-Wall" "-O2")

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)