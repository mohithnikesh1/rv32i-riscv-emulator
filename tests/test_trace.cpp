#include <sstream>

#include <gtest/gtest.h>

#include "emulator/cpu.hpp"
#include "emulator/decoder.hpp"
#include "emulator/memory.hpp"
#include "emulator/trace.hpp"

// ---------------------------------------------------------------------------
// dump_registers
// ---------------------------------------------------------------------------

TEST(DumpRegisters, AllZeroByDefault) {
    CPUState cpu;
    std::ostringstream out;
    dump_registers(cpu, out);
    const std::string s = out.str();
    // Every register value should appear as 0x00000000
    EXPECT_NE(s.find("0x00000000"), std::string::npos);
    // Header line must be present
    EXPECT_NE(s.find("Registers:"), std::string::npos);
}

TEST(DumpRegisters, ShowsNonZeroValue) {
    CPUState cpu;
    cpu.set_reg(1, 0xDEADBEEF);
    std::ostringstream out;
    dump_registers(cpu, out);
    EXPECT_NE(out.str().find("0xDEADBEEF"), std::string::npos);
}

TEST(DumpRegisters, ContainsAbiNames) {
    CPUState cpu;
    std::ostringstream out;
    dump_registers(cpu, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("zero"), std::string::npos);
    EXPECT_NE(s.find("ra"),   std::string::npos);
    EXPECT_NE(s.find("sp"),   std::string::npos);
    EXPECT_NE(s.find("t6"),   std::string::npos);
}

TEST(DumpRegisters, ContainsAllRegisterNumbers) {
    CPUState cpu;
    std::ostringstream out;
    dump_registers(cpu, out);
    const std::string s = out.str();
    // Spot-check a few register numbers
    EXPECT_NE(s.find("x0"),  std::string::npos);
    EXPECT_NE(s.find("x15"), std::string::npos);
    EXPECT_NE(s.find("x31"), std::string::npos);
}

TEST(DumpRegisters, X0AlwaysZero) {
    CPUState cpu;
    // set_reg on x0 is a no-op, so x0 stays 0
    cpu.set_reg(0, 0xFFFFFFFF);
    std::ostringstream out;
    dump_registers(cpu, out);
    // The very first register line must show x0 = 0
    const std::string s = out.str();
    EXPECT_NE(s.find("x0"), std::string::npos);
    // Ensure 0xFFFFFFFF does NOT appear (x0 can't hold it)
    EXPECT_EQ(s.find("0xFFFFFFFF"), std::string::npos);
}

// ---------------------------------------------------------------------------
// trace_instruction – R-type
// ---------------------------------------------------------------------------

TEST(TraceInstruction, RTypeADD) {
    // ADD x3, x1, x2  ->  0x002081B3
    uint32_t enc = 0x002081B3u;
    DecodedInstruction instr = decode(enc);
    std::ostringstream out;
    trace_instruction(0x00000000, enc, instr, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("PC=0x00000000"), std::string::npos);
    EXPECT_NE(s.find("0x002081B3"),    std::string::npos);
    EXPECT_NE(s.find("ADD"),           std::string::npos);
    EXPECT_NE(s.find("x3"),            std::string::npos);
    EXPECT_NE(s.find("x1"),            std::string::npos);
    EXPECT_NE(s.find("x2"),            std::string::npos);
}

// ---------------------------------------------------------------------------
// trace_instruction – I-type arithmetic
// ---------------------------------------------------------------------------

TEST(TraceInstruction, ADDI) {
    // ADDI x1, x0, 5  ->  0x00500093
    uint32_t enc = 0x00500093u;
    DecodedInstruction instr = decode(enc);
    std::ostringstream out;
    trace_instruction(0x00000004, enc, instr, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("PC=0x00000004"), std::string::npos);
    EXPECT_NE(s.find("ADDI"),          std::string::npos);
    EXPECT_NE(s.find("x1"),            std::string::npos);
    EXPECT_NE(s.find("x0"),            std::string::npos);
    EXPECT_NE(s.find("5"),             std::string::npos);
}

TEST(TraceInstruction, ADDINegativeImmediate) {
    // ADDI x1, x0, -1  ->  imm = 0xFFF, opcode 0x13, funct3 0, rd=1, rs1=0
    // encoding: 0xFFF00093
    uint32_t enc = 0xFFF00093u;
    DecodedInstruction instr = decode(enc);
    std::ostringstream out;
    trace_instruction(0x00000008, enc, instr, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("ADDI"), std::string::npos);
    EXPECT_NE(s.find("-1"),   std::string::npos);
}

// ---------------------------------------------------------------------------
// trace_instruction – Load
// ---------------------------------------------------------------------------

TEST(TraceInstruction, LWFormat) {
    // LW x2, 8(x1)  ->  opcode=0x03, funct3=2, rd=2, rs1=1, imm=8
    // bits: imm[11:0]=0x008, rs1=1, funct3=2, rd=2, opcode=0x03
    // = (0x008 << 20) | (1 << 15) | (2 << 12) | (2 << 7) | 0x03
    uint32_t enc = (0x008u << 20) | (1u << 15) | (2u << 12) | (2u << 7) | 0x03u;
    DecodedInstruction instr = decode(enc);
    std::ostringstream out;
    trace_instruction(0x0000000C, enc, instr, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("LW"),   std::string::npos);
    EXPECT_NE(s.find("x2"),   std::string::npos);
    EXPECT_NE(s.find("8("),   std::string::npos);  // imm(rs1) format
    EXPECT_NE(s.find("x1"),   std::string::npos);
}

// ---------------------------------------------------------------------------
// trace_instruction – Store
// ---------------------------------------------------------------------------

TEST(TraceInstruction, SWFormat) {
    // SW x2, 4(x1)  ->  opcode=0x23, funct3=2, rs1=1, rs2=2, imm=4
    // S-type: imm[11:5] in bits[31:25], imm[4:0] in bits[11:7]
    // imm=4 -> imm[11:5]=0, imm[4:0]=4
    uint32_t enc = (0u << 25) | (2u << 20) | (1u << 15) | (2u << 12) | (4u << 7) | 0x23u;
    DecodedInstruction instr = decode(enc);
    std::ostringstream out;
    trace_instruction(0x00000010, enc, instr, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("SW"),  std::string::npos);
    EXPECT_NE(s.find("x2"),  std::string::npos);
    EXPECT_NE(s.find("4("),  std::string::npos);
    EXPECT_NE(s.find("x1"),  std::string::npos);
}

// ---------------------------------------------------------------------------
// trace_instruction – Branch
// ---------------------------------------------------------------------------

TEST(TraceInstruction, BEQFormat) {
    // BEQ x1, x2, 8
    // B-type: imm=8 -> bit12=0, bit11=0, imm[10:5]=0, imm[4:1]=4(0b0100)
    // bits[31]=0, bits[30:25]=0, bits[24:20]=rs2=2, bits[19:15]=rs1=1,
    // bits[14:12]=funct3=0, bits[11:8]=imm[4:1]=4, bit[7]=imm[11]=0, bits[6:0]=0x63
    uint32_t enc = (0u << 31) | (0u << 25) | (2u << 20) | (1u << 15) |
                   (0u << 12) | (4u << 8)  | (0u << 7)  | 0x63u;
    DecodedInstruction instr = decode(enc);
    std::ostringstream out;
    trace_instruction(0x00000014, enc, instr, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("BEQ"), std::string::npos);
    EXPECT_NE(s.find("x1"),  std::string::npos);
    EXPECT_NE(s.find("x2"),  std::string::npos);
    EXPECT_NE(s.find("8"),   std::string::npos);
}

// ---------------------------------------------------------------------------
// trace_instruction – JAL
// ---------------------------------------------------------------------------

TEST(TraceInstruction, JALFormat) {
    // JAL x1, 0  (jump to self, save ra)
    // J-type imm=0: all imm bits zero; rd=1, opcode=0x6F
    uint32_t enc = (1u << 7) | 0x6Fu;
    DecodedInstruction instr = decode(enc);
    std::ostringstream out;
    trace_instruction(0x00000000, enc, instr, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("JAL"), std::string::npos);
    EXPECT_NE(s.find("x1"),  std::string::npos);
    EXPECT_NE(s.find("0"),   std::string::npos);
}

// ---------------------------------------------------------------------------
// trace_instruction – JALR
// ---------------------------------------------------------------------------

TEST(TraceInstruction, JALRFormat) {
    // JALR x0, x1, 0  (return)
    // opcode=0x67, funct3=0, rd=0, rs1=1, imm=0
    uint32_t enc = (1u << 15) | 0x67u;
    DecodedInstruction instr = decode(enc);
    std::ostringstream out;
    trace_instruction(0x00000018, enc, instr, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("JALR"), std::string::npos);
    EXPECT_NE(s.find("x0"),   std::string::npos);
    EXPECT_NE(s.find("x1"),   std::string::npos);
}

// ---------------------------------------------------------------------------
// Non-interference: trace writes only to the given ostream
// ---------------------------------------------------------------------------

TEST(TraceInstruction, DoesNotModifyCPUState) {
    CPUState cpu;
    Memory   mem(1024);
    cpu.set_reg(1, 42);
    cpu.pc = 0x10;

    uint32_t enc = 0x00500093u;  // ADDI x1, x0, 5
    DecodedInstruction instr = decode(enc);
    std::ostringstream out;
    trace_instruction(cpu.pc, enc, instr, out);

    // trace must not alter cpu
    EXPECT_EQ(cpu.get_reg(1), 42u);
    EXPECT_EQ(cpu.pc, 0x10u);
}
