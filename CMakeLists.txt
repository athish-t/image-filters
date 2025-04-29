cmake_minimum_required(VERSION 3.10)

# Set the project name
project(image_filters)

# Use Ninja as the generator
set(CMAKE_GENERATOR "Ninja")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

# Set the build type
set(CMAKE_BUILD_TYPE Debug)

# Add debug information flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Find OpenCV
find_package(OpenCV REQUIRED)

# Find TBB
find_package(TBB REQUIRED)

# Include OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

# Add all source files in the src directory except main.cpp for the library
file(GLOB LIB_SRC_FILES src/*.cpp)
list(FILTER LIB_SRC_FILES EXCLUDE REGEX "src/main.cpp")

# Create a shared library from the source files
add_library(${PROJECT_NAME}_lib SHARED ${LIB_SRC_FILES})

# Set library properties
set_target_properties(${PROJECT_NAME}_lib PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    VERSION 1.0.0
    SOVERSION 1)

# Link the library with OpenCV and TBB
target_link_libraries(${PROJECT_NAME}_lib
    ${OpenCV_LIBS}
    TBB::tbb)

# Add the main executable
add_executable(main src/main.cpp)

# Link the main executable with our library
target_link_libraries(main ${PROJECT_NAME}_lib)

# Add Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add test executables
file(GLOB TEST_FILES test/*.cpp)
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})

    # Link test executable with Google Test and our library
    target_link_libraries(${TEST_NAME}
        GTest::GTest
        GTest::Main
        ${PROJECT_NAME}_lib)

    # Add test to CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
