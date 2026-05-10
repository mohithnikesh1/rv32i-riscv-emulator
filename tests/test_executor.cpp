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
