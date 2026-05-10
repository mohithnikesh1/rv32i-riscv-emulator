cmake_minimum_required(VERSION 3.20)
project(riscv_emulator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch GoogleTest for unit testing
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

# Shared library: cpu and memory (linked by main and tests)
add_library(emulator_lib
    src/cpu.cpp
    src/memory.cpp
    src/decoder.cpp
    src/executor.cpp
)
target_include_directories(emulator_lib PUBLIC include)

# Main emulator executable
add_executable(riscv_emulator src/main.cpp)
target_link_libraries(riscv_emulator PRIVATE emulator_lib)

# Tests
enable_testing()
add_subdirectory(tests)
