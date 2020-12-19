cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(LCFExample VERSION 1.0.0)

# Source files
add_executable(LCFExample
                "${CMAKE_CURRENT_LIST_DIR}/main.cpp" 
)

# Build libraries
include("${LIBLAZYCONFIG_DIR}/Source/CMakeLists.txt")

# Includes
target_include_directories(LCFExample PUBLIC
                          "${CMAKE_CURRENT_LIST_DIR}"
                          "${LIBLAZYCONFIG_DIR}/Source"
)

# Libraries
# if Windows
if (WIN32)
link_directories("${LIBLAZYCONFIG_DIR}/build")
target_link_libraries(LCFExample 
                        "${LIBLAZYCONFIG_DIR}/build/libLazyConfig.dll.a"
)
# If not
else()
target_link_libraries(LCFExample 
                        "${LIBLAZYCONFIG_DIR}/build/libLazyConfig.so"
)
endif()

# Flags
target_compile_options(LCFExample PUBLIC "-Wall" "-O2")

# Wait first for libraries to build
add_dependencies(LCFExample LazyConfig)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)