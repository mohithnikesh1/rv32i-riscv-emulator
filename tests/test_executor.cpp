#include <gtest/gtest.h>

#include "emulator/cpu.hpp"
#include "emulator/decoder.hpp"
#include "emulator/executor.hpp"
#include "emulator/memory.hpp"

// Helper: decode then execute one instruction on a CPU with memory.
static void run(CPUState& cpu, Memory& mem, uint32_t encoding) {
    execute(decode(encoding), cpu, mem);
}

// ---------------------------------------------------------------------------
// ADDI tests
// ---------------------------------------------------------------------------

// ADDI x1, x0, 5  ->  x1 = 0 + 5 = 5
//   000000000101 | 00000 | 000 | 00001 | 0010011
//   imm=5, rs1=x0, funct3=0, rd=x1, opcode=0x13
//   Encoding: 0x00500093
TEST(ExecutorTest, AddiBasic) {
    CPUState cpu;
    Memory mem(1024);
    run(cpu, mem, 0x00500093);
    EXPECT_EQ(cpu.get_reg(1), 5u);
}

// Set x1=10, then ADDI x2, x1, -1  ->  x2 = 10 + (-1) = 9
//   111111111111 | 00001 | 000 | 00010 | 0010011
//   imm=-1, rs1=x1, funct3=0, rd=x2, opcode=0x13
//   Encoding: 0xFFF08113
TEST(ExecutorTest, AddiNegativeImmediate) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 10);
    run(cpu, mem, 0xFFF08113);
    EXPECT_EQ(cpu.get_reg(2), 9u);
}

// ADDI x0, x1, 10  ->  x0 must stay 0 (x0 is hardwired zero)
//   000000001010 | 00001 | 000 | 00000 | 0010011
//   imm=10, rs1=x1, funct3=0, rd=x0, opcode=0x13
//   Encoding: 0x00A08013
TEST(ExecutorTest, AddiWriteToX0DoesNothing) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 99);
    run(cpu, mem, 0x00A08013);
    EXPECT_EQ(cpu.get_reg(0), 0u);
}

TEST(ExecutorTest, AddiAdvancesPCByFour) {
    CPUState cpu;
    Memory mem(1024);
    EXPECT_EQ(cpu.pc, 0u);
    run(cpu, mem, 0x00500093);
    EXPECT_EQ(cpu.pc, 4u);
}

// ---------------------------------------------------------------------------
// R-type tests (x1=10, x2=3 for all)
//
// R-type encoding layout:
//   funct7 | rs2 | rs1 | funct3 | rd | opcode(0x33)
// ---------------------------------------------------------------------------

// ADD x3, x1, x2  ->  x3 = 10 + 3 = 13
//   0000000 | 00010 | 00001 | 000 | 00011 | 0110011  ->  0x002081B3
TEST(ExecutorTest, Add) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, mem, 0x002081B3);
    EXPECT_EQ(cpu.get_reg(3), 13u);
}

// SUB x3, x1, x2  ->  x3 = 10 - 3 = 7
//   0100000 | 00010 | 00001 | 000 | 00011 | 0110011  ->  0x402081B3
TEST(ExecutorTest, Sub) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, mem, 0x402081B3);
    EXPECT_EQ(cpu.get_reg(3), 7u);
}

// AND x3, x1, x2  ->  x3 = 10 & 3 = 0b1010 & 0b0011 = 2
//   0000000 | 00010 | 00001 | 111 | 00011 | 0110011  ->  0x0020F1B3
TEST(ExecutorTest, And) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, mem, 0x0020F1B3);
    EXPECT_EQ(cpu.get_reg(3), 2u);
}

// OR x3, x1, x2  ->  x3 = 10 | 3 = 0b1010 | 0b0011 = 11
//   0000000 | 00010 | 00001 | 110 | 00011 | 0110011  ->  0x0020E1B3
TEST(ExecutorTest, Or) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, mem, 0x0020E1B3);
    EXPECT_EQ(cpu.get_reg(3), 11u);
}

// XOR x3, x1, x2  ->  x3 = 10 ^ 3 = 0b1010 ^ 0b0011 = 9
//   0000000 | 00010 | 00001 | 100 | 00011 | 0110011  ->  0x0020C1B3
TEST(ExecutorTest, Xor) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, mem, 0x0020C1B3);
    EXPECT_EQ(cpu.get_reg(3), 9u);
}

// ADD x0, x1, x2  ->  x0 must stay 0 (x0 is hardwired zero)
//   0000000 | 00010 | 00001 | 000 | 00000 | 0110011  ->  0x00208033
TEST(ExecutorTest, AddWriteToX0DoesNothing) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    run(cpu, mem, 0x00208033);
    EXPECT_EQ(cpu.get_reg(0), 0u);
}

TEST(ExecutorTest, RTypeAdvancesPCByFour) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 10);
    cpu.set_reg(2, 3);
    EXPECT_EQ(cpu.pc, 0u);
    run(cpu, mem, 0x002081B3);  // ADD x3, x1, x2
    EXPECT_EQ(cpu.pc, 4u);
}

// ---------------------------------------------------------------------------
// Load / store tests
//
// Load encoding (I-type):
//   imm[11:0] | rs1 | funct3 | rd | opcode(0x03)
//
// Store encoding (S-type):
//   imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode(0x23)
// ---------------------------------------------------------------------------

// SW x1, 0(x0) then LW x2, 0(x0)  ->  x2 should equal original x1 value
//
//   SW: 0000000 | 00001 | 00000 | 010 | 00000 | 0100011  ->  0x00102023
//   LW: 000000000000 | 00000 | 010 | 00010 | 0000011      ->  0x00002103
TEST(ExecutorTest, SwThenLw) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 0xDEADBEEF);
    run(cpu, mem, 0x00102023);  // SW x1, 0(x0)
    run(cpu, mem, 0x00002103);  // LW x2, 0(x0)
    EXPECT_EQ(cpu.get_reg(2), 0xDEADBEEFu);
}

// SB x1, 0(x0) stores only the lowest byte of x1.
//
//   SB: 0000000 | 00001 | 00000 | 000 | 00000 | 0100011  ->  0x00100023
TEST(ExecutorTest, SbStoresOnlyLowByte) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 0xABCDEF12);
    run(cpu, mem, 0x00100023);  // SB x1, 0(x0)
    EXPECT_EQ(mem.read8(0), 0x12u);
    // Byte at address 1 must be untouched
    EXPECT_EQ(mem.read8(1), 0x00u);
}

// SH x1, 0(x0) stores only the lowest halfword of x1.
//
//   SH: 0000000 | 00001 | 00000 | 001 | 00000 | 0100011  ->  0x00101023
TEST(ExecutorTest, ShStoresOnlyLowHalfword) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 0xABCDEF12);
    run(cpu, mem, 0x00101023);  // SH x1, 0(x0)
    EXPECT_EQ(mem.read16(0), 0xEF12u);
    // Byte at address 2 must be untouched
    EXPECT_EQ(mem.read8(2), 0x00u);
}

// LB sign-extends the loaded byte into a full 32-bit register.
// 0xFF is -1 as an 8-bit signed value; after sign extension it should be 0xFFFFFFFF.
//
//   LB: 000000000000 | 00000 | 000 | 00001 | 0000011  ->  0x00000083
TEST(ExecutorTest, LbSignExtends) {
    CPUState cpu;
    Memory mem(1024);
    mem.write8(0, 0xFF);
    run(cpu, mem, 0x00000083);  // LB x1, 0(x0)
    EXPECT_EQ(cpu.get_reg(1), 0xFFFFFFFFu);
}

// LH sign-extends the loaded halfword into a full 32-bit register.
// 0x8000 is -32768 as a 16-bit signed value; after sign extension: 0xFFFF8000.
//
//   LH: 000000000000 | 00000 | 001 | 00001 | 0000011  ->  0x00001083
TEST(ExecutorTest, LhSignExtends) {
    CPUState cpu;
    Memory mem(1024);
    mem.write16(0, 0x8000);
    run(cpu, mem, 0x00001083);  // LH x1, 0(x0)
    EXPECT_EQ(cpu.get_reg(1), 0xFFFF8000u);
}

// LW x0, 0(x0)  ->  x0 must stay 0 even after a load.
//
//   LW: 000000000000 | 00000 | 010 | 00000 | 0000011  ->  0x00002003
TEST(ExecutorTest, LwToX0DoesNothing) {
    CPUState cpu;
    Memory mem(1024);
    mem.write32(0, 0x12345678);
    run(cpu, mem, 0x00002003);  // LW x0, 0(x0)
    EXPECT_EQ(cpu.get_reg(0), 0u);
}

// Load/store instructions advance PC by 4.
TEST(ExecutorTest, LoadStoreAdvancesPCByFour) {
    CPUState cpu;
    Memory mem(1024);
    EXPECT_EQ(cpu.pc, 0u);
    run(cpu, mem, 0x00002083);  // LW x1, 0(x0)
    EXPECT_EQ(cpu.pc, 4u);
}

// An out-of-bounds load throws std::out_of_range (propagated from Memory).
// Set x1 = 1021, then LW x2, 0(x1): bytes [1021..1024] straddle the 1024-byte boundary.
//
//   LW x2, 0(x1): 000000000000 | 00001 | 010 | 00010 | 0000011  ->  0x0000A103
TEST(ExecutorTest, OutOfBoundsLoadThrows) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 1021);
    EXPECT_THROW(run(cpu, mem, 0x0000A103), std::out_of_range);
}
