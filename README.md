# Sobel Filter Project

This project demonstrates the use of OpenCV with the Ninja build system to create a simple application. The application initializes an OpenCV matrix and performs basic operations.

## Prerequisites

- OpenCV library
- CMake (with Ninja generator)
- A C++ compiler

## Build Instructions

1. Clone the repository:
   ```bash
   git clone git@github.com:athish-t/sobel-filter.git
   cd sobel-filter
   ```

2. Create a build directory and configure the project:
   ```bash
   mkdir build
   cd build
   cmake -G Ninja ..
   ```

3. Build the project:
   ```bash
   cmake --build .
   ```

## Run the Application

After building, run the application:
```bash
./build/main
```

## Project Structure

- `src/`: Contains the source code.
- `include/`: Contains header files.
- `build/`: Contains build artifacts.

## License

This project is licensed under the MIT License.
