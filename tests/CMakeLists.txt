add_executable(test_cpu test_cpu.cpp)
target_link_libraries(test_cpu PRIVATE emulator_lib GTest::gtest_main)
add_test(NAME test_cpu COMMAND test_cpu)

add_executable(test_memory test_memory.cpp)
target_link_libraries(test_memory PRIVATE emulator_lib GTest::gtest_main)
add_test(NAME test_memory COMMAND test_memory)
