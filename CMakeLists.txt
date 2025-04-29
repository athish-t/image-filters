cmake_minimum_required(VERSION 3.10)

# Set the project name
project(MyNinjaProject)

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

# Include OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

# Add all source files in the src directory
file(GLOB SRC_FILES src/*.cpp)
add_executable(main ${SRC_FILES})

# Link OpenCV libraries
target_link_libraries(main ${OpenCV_LIBS})
find_package(TBB REQUIRED)
target_link_libraries(main TBB::tbb)

# Add Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Exclude main.cpp from test executables
list(FILTER SRC_FILES EXCLUDE REGEX "src/main.cpp")

# Add test executables
file(GLOB TEST_FILES test/*.cpp)
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ${SRC_FILES})

    # Link test executable with Google Test and project libraries
    target_link_libraries(${TEST_NAME} GTest::GTest GTest::Main)
    target_link_libraries(${TEST_NAME} TBB::tbb)
    target_link_libraries(${TEST_NAME} ${OpenCV_LIBS})

    # Add test to CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
