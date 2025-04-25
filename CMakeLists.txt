cmake_minimum_required(VERSION 3.10)

# Set the project name
project(MyNinjaProject)

# Use Ninja as the generator
set(CMAKE_GENERATOR "Ninja")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Set the build type
set(CMAKE_BUILD_TYPE Debug)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

# Add the source files
add_executable(main src/main.cpp)

# Link OpenCV libraries
target_link_libraries(main ${OpenCV_LIBS})
