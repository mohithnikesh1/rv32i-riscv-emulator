#include <gtest/gtest.h>

#include "emulator/cpu.hpp"

TEST(CPUStateTest, AllRegistersStartAtZero) {
    CPUState cpu;
    for (int i = 0; i < 32; i++) {
        EXPECT_EQ(cpu.get_reg(i), 0u) << "register x" << i << " should start at zero";
    }
}

TEST(CPUStateTest, PCStartsAtZero) {
    CPUState cpu;
    EXPECT_EQ(cpu.pc, 0u);
}

TEST(CPUStateTest, X0AlwaysRemainsZero) {
    CPUState cpu;
    cpu.set_reg(0, 42);
    EXPECT_EQ(cpu.get_reg(0), 0u) << "x0 must never change";
}

TEST(CPUStateTest, SetAndGetRegister) {
    CPUState cpu;
    cpu.set_reg(1, 100);
    EXPECT_EQ(cpu.get_reg(1), 100u);
}

TEST(CPUStateTest, SetMultipleRegisters) {
    CPUState cpu;
    cpu.set_reg(5, 0xDEAD);
    cpu.set_reg(6, 0xBEEF);
    EXPECT_EQ(cpu.get_reg(5), 0xDEADu);
    EXPECT_EQ(cpu.get_reg(6), 0xBEEFu);
}

TEST(CPUStateTest, ResetClearsAllState) {
    CPUState cpu;
    cpu.set_reg(5, 999);
    cpu.pc = 100;
    cpu.reset();
    EXPECT_EQ(cpu.get_reg(5), 0u);
    EXPECT_EQ(cpu.pc, 0u);
}
