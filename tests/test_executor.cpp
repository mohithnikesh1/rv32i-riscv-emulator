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

// ---------------------------------------------------------------------------
// Branch tests
//
// B-type encoding layout:
//   imm[12] | imm[10:5] | rs2 | rs1 | funct3 | imm[4:1] | imm[11] | opcode(0x63)
//
// All tests use rs1=x1, rs2=x2, offset=+8 (so taken -> PC=8, not taken -> PC=4).
//
//   BEQ x1,x2,8: 0|000000|00010|00001|000|0100|0|1100011  ->  0x00208463
//   BNE x1,x2,8: 0|000000|00010|00001|001|0100|0|1100011  ->  0x00209463
//   BLT x1,x2,8: 0|000000|00010|00001|100|0100|0|1100011  ->  0x0020C463
//   BGE x1,x2,8: 0|000000|00010|00001|101|0100|0|1100011  ->  0x0020D463
// ---------------------------------------------------------------------------

// BEQ: branch when rs1 == rs2
TEST(ExecutorTest, BeqTaken) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 5);
    cpu.set_reg(2, 5);
    run(cpu, mem, 0x00208463);  // BEQ x1, x2, 8
    EXPECT_EQ(cpu.pc, 8u);
}

TEST(ExecutorTest, BeqNotTaken) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 5);
    cpu.set_reg(2, 3);
    run(cpu, mem, 0x00208463);  // BEQ x1, x2, 8
    EXPECT_EQ(cpu.pc, 4u);
}

// BNE: branch when rs1 != rs2
TEST(ExecutorTest, BneTaken) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 5);
    cpu.set_reg(2, 3);
    run(cpu, mem, 0x00209463);  // BNE x1, x2, 8
    EXPECT_EQ(cpu.pc, 8u);
}

TEST(ExecutorTest, BneNotTaken) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 5);
    cpu.set_reg(2, 5);
    run(cpu, mem, 0x00209463);  // BNE x1, x2, 8
    EXPECT_EQ(cpu.pc, 4u);
}

// BLT: branch when rs1 < rs2 (signed).
// Use x1=-1 (0xFFFFFFFF), x2=5: signed -1 < 5 -> taken.
TEST(ExecutorTest, BltTakenSigned) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, static_cast<uint32_t>(-1));  // -1 in two's complement
    cpu.set_reg(2, 5);
    run(cpu, mem, 0x0020C463);  // BLT x1, x2, 8
    EXPECT_EQ(cpu.pc, 8u);
}

// x1=5, x2=-1: signed 5 >= -1 -> not taken.
TEST(ExecutorTest, BltNotTakenSigned) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 5);
    cpu.set_reg(2, static_cast<uint32_t>(-1));
    run(cpu, mem, 0x0020C463);  // BLT x1, x2, 8
    EXPECT_EQ(cpu.pc, 4u);
}

// BGE: branch when rs1 >= rs2 (signed).
// x1=5, x2=-1: signed 5 >= -1 -> taken.
TEST(ExecutorTest, BgeTakenSigned) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, 5);
    cpu.set_reg(2, static_cast<uint32_t>(-1));
    run(cpu, mem, 0x0020D463);  // BGE x1, x2, 8
    EXPECT_EQ(cpu.pc, 8u);
}

// x1=-1, x2=5: signed -1 < 5 -> not taken.
TEST(ExecutorTest, BgeNotTakenSigned) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(1, static_cast<uint32_t>(-1));
    cpu.set_reg(2, 5);
    run(cpu, mem, 0x0020D463);  // BGE x1, x2, 8
    EXPECT_EQ(cpu.pc, 4u);
}

// PC arithmetic: taken branch from a non-zero PC should land at PC + imm.
// Start at PC=100, take BEQ with offset 8 -> PC should be 108.
TEST(ExecutorTest, BranchTakenUpdatesPC) {
    CPUState cpu;
    Memory mem(1024);
    cpu.pc = 100;
    cpu.set_reg(1, 7);
    cpu.set_reg(2, 7);
    run(cpu, mem, 0x00208463);  // BEQ x1, x2, 8
    EXPECT_EQ(cpu.pc, 108u);
}

// Not-taken branch from a non-zero PC should land at PC + 4.
// Start at PC=100, BEQ not taken -> PC should be 104.
TEST(ExecutorTest, BranchNotTakenUpdatesPC) {
    CPUState cpu;
    Memory mem(1024);
    cpu.pc = 100;
    cpu.set_reg(1, 7);
    cpu.set_reg(2, 9);
    run(cpu, mem, 0x00208463);  // BEQ x1, x2, 8
    EXPECT_EQ(cpu.pc, 104u);
}

// ---------------------------------------------------------------------------
// JAL / JALR tests
//
// JAL encoding (J-type):
//   imm[20] | imm[10:1] | imm[11] | imm[19:12] | rd | opcode(0x6F)
//
// JALR encoding (I-type):
//   imm[11:0] | rs1 | funct3(000) | rd | opcode(0x67)
//
//   JAL  x1, +8  : 0x008000EF   (rd=x1, offset=+8)
//   JAL  x0, +8  : 0x0080006F   (rd=x0, offset=+8)
//   JALR x1,x2,4 : 0x004100E7   (rd=x1, rs1=x2, imm=4)
//   JALR x0,x2,4 : 0x00410067   (rd=x0, rs1=x2, imm=4)
//   JALR x1,x2,0 : 0x000100E7   (rd=x1, rs1=x2, imm=0)
// ---------------------------------------------------------------------------

// JAL must write the return address (PC + 4) into rd.
// Start at PC=0, JAL x1, +8 -> x1 should be 4, PC should be 8.
TEST(ExecutorTest, JalStoresReturnAddress) {
    CPUState cpu;
    Memory mem(1024);
    run(cpu, mem, 0x008000EF);  // JAL x1, +8
    EXPECT_EQ(cpu.get_reg(1), 4u);   // return addr = 0 + 4
    EXPECT_EQ(cpu.pc, 8u);
}

// JAL must add the immediate to the current PC, not always jump to a fixed address.
// Start at PC=100, JAL x1, +8 -> PC should be 108.
TEST(ExecutorTest, JalUpdatesPCRelative) {
    CPUState cpu;
    Memory mem(1024);
    cpu.pc = 100;
    run(cpu, mem, 0x008000EF);  // JAL x1, +8
    EXPECT_EQ(cpu.pc, 108u);
    EXPECT_EQ(cpu.get_reg(1), 104u);  // return addr = 100 + 4
}

// JAL with rd=x0 must not change x0, but the jump still happens.
TEST(ExecutorTest, JalToX0DoesNotChangeX0) {
    CPUState cpu;
    Memory mem(1024);
    run(cpu, mem, 0x0080006F);  // JAL x0, +8
    EXPECT_EQ(cpu.get_reg(0), 0u);
    EXPECT_EQ(cpu.pc, 8u);
}

// JALR must write the return address (PC + 4) into rd.
// Set x2=100, PC=0, JALR x1,x2,4 -> x1 = 4, PC = (100+4)&~1 = 104.
TEST(ExecutorTest, JalrStoresReturnAddress) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(2, 100);
    run(cpu, mem, 0x004100E7);  // JALR x1, x2, 4
    EXPECT_EQ(cpu.get_reg(1), 4u);   // return addr = 0 + 4
    EXPECT_EQ(cpu.pc, 104u);
}

// JALR target is rs1 + imm (absolute, not PC-relative).
// Set x2=200, PC=0, JALR x1,x2,4 -> PC = (200+4)&~1 = 204.
TEST(ExecutorTest, JalrUpdatesPC) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(2, 200);
    run(cpu, mem, 0x004100E7);  // JALR x1, x2, 4
    EXPECT_EQ(cpu.pc, 204u);
}

// JALR must clear the lowest bit of the computed target address.
// Set x2=5 (odd), imm=0: target = (5+0)&~1 = 4.
TEST(ExecutorTest, JalrClearsLowestBit) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(2, 5);
    run(cpu, mem, 0x000100E7);  // JALR x1, x2, 0
    EXPECT_EQ(cpu.pc, 4u);
}

// JALR with rd=x0 must not change x0, but the jump still happens.
TEST(ExecutorTest, JalrToX0DoesNotChangeX0) {
    CPUState cpu;
    Memory mem(1024);
    cpu.set_reg(2, 100);
    run(cpu, mem, 0x00410067);  // JALR x0, x2, 4
    EXPECT_EQ(cpu.get_reg(0), 0u);
    EXPECT_EQ(cpu.pc, 104u);
}
