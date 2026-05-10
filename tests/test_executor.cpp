#include <gtest/gtest.h>

#include "emulator/cpu.hpp"
#include "emulator/decoder.hpp"
#include "emulator/executor.hpp"

// Helper: decode then execute one instruction on a CPU.
static void run(CPUState& cpu, uint32_t encoding) {
    execute(decode(encoding), cpu);
}

// ADDI x1, x0, 5  ->  x1 = 0 + 5 = 5
//
//   000000000101 | 00000 | 000 | 00001 | 0010011
//   imm=5, rs1=x0, funct3=0, rd=x1, opcode=0x13
//   Encoding: 0x00500093
TEST(ExecutorTest, AddiBasic) {
    CPUState cpu;
    run(cpu, 0x00500093);
    EXPECT_EQ(cpu.get_reg(1), 5u);
}

// Set x1=10, then ADDI x2, x1, -1  ->  x2 = 10 + (-1) = 9
//
//   111111111111 | 00001 | 000 | 00010 | 0010011
//   imm=-1, rs1=x1, funct3=0, rd=x2, opcode=0x13
//   Encoding: 0xFFF08113
TEST(ExecutorTest, AddiNegativeImmediate) {
    CPUState cpu;
    cpu.set_reg(1, 10);
    run(cpu, 0xFFF08113);
    EXPECT_EQ(cpu.get_reg(2), 9u);
}

// ADDI x0, x1, 10  ->  x0 must stay 0 (x0 is hardwired zero)
//
//   000000001010 | 00001 | 000 | 00000 | 0010011
//   imm=10, rs1=x1, funct3=0, rd=x0, opcode=0x13
//   Encoding: 0x00A08013
TEST(ExecutorTest, AddiWriteToX0DoesNothing) {
    CPUState cpu;
    cpu.set_reg(1, 99);
    run(cpu, 0x00A08013);
    EXPECT_EQ(cpu.get_reg(0), 0u);
}

// Each executed instruction must advance PC by exactly 4.
TEST(ExecutorTest, AddiAdvancesPCByFour) {
    CPUState cpu;
    EXPECT_EQ(cpu.pc, 0u);
    run(cpu, 0x00500093);
    EXPECT_EQ(cpu.pc, 4u);
}

// --- R-type tests (x1=10, x2=3 for all unless noted) ---
//
// R-type encoding layout:
//   funct7 | rs2 | rs1 | funct3 | rd | opcode(0x33)
//
// ADD x3, x1, x2  ->  x3 = 10 + 3 = 13
//   0000000 | 00010 | 00001 | 000 | 00011 | 0110011
//   Encoding: 0x002081B3
TEST(ExecutorTest, Add) {
    CPUState cpu;
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, 0x002081B3);
    EXPECT_EQ(cpu.get_reg(3), 13u);
}

// SUB x3, x1, x2  ->  x3 = 10 - 3 = 7
//   0100000 | 00010 | 00001 | 000 | 00011 | 0110011
//   Encoding: 0x402081B3
TEST(ExecutorTest, Sub) {
    CPUState cpu;
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, 0x402081B3);
    EXPECT_EQ(cpu.get_reg(3), 7u);
}

// AND x3, x1, x2  ->  x3 = 10 & 3 = 0b1010 & 0b0011 = 0b0010 = 2
//   0000000 | 00010 | 00001 | 111 | 00011 | 0110011
//   Encoding: 0x0020F1B3
TEST(ExecutorTest, And) {
    CPUState cpu;
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, 0x0020F1B3);
    EXPECT_EQ(cpu.get_reg(3), 2u);
}

// OR x3, x1, x2  ->  x3 = 10 | 3 = 0b1010 | 0b0011 = 0b1011 = 11
//   0000000 | 00010 | 00001 | 110 | 00011 | 0110011
//   Encoding: 0x0020E1B3
TEST(ExecutorTest, Or) {
    CPUState cpu;
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, 0x0020E1B3);
    EXPECT_EQ(cpu.get_reg(3), 11u);
}

// XOR x3, x1, x2  ->  x3 = 10 ^ 3 = 0b1010 ^ 0b0011 = 0b1001 = 9
//   0000000 | 00010 | 00001 | 100 | 00011 | 0110011
//   Encoding: 0x0020C1B3
TEST(ExecutorTest, Xor) {
    CPUState cpu;
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, 0x0020C1B3);
    EXPECT_EQ(cpu.get_reg(3), 9u);
}

// ADD x0, x1, x2  ->  x0 must stay 0 (x0 is hardwired zero)
//   0000000 | 00010 | 00001 | 000 | 00000 | 0110011
//   Encoding: 0x00208033
TEST(ExecutorTest, AddWriteToX0DoesNothing) {
    CPUState cpu;
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, 0x00208033);
    EXPECT_EQ(cpu.get_reg(0), 0u);
}

// R-type instruction must also advance PC by 4.
TEST(ExecutorTest, RTypeAdvancesPCByFour) {
    CPUState cpu;
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    EXPECT_EQ(cpu.pc, 0u);
    run(cpu, 0x002081B3);  // ADD x3, x1, x2
    EXPECT_EQ(cpu.pc, 4u);
}
