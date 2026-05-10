#include "emulator/trace.hpp"

#include <array>
#include <cstdio>
#include <ostream>
#include <string>

static constexpr std::array<const char*, 32> ABI_NAMES = {
    "zero", "ra", "sp",  "gp",  "tp",  "t0", "t1", "t2",
    "s0",   "s1", "a0",  "a1",  "a2",  "a3", "a4", "a5",
    "a6",   "a7", "s2",  "s3",  "s4",  "s5", "s6", "s7",
    "s8",   "s9", "s10", "s11", "t3",  "t4", "t5", "t6",
};

void dump_registers(const CPUState& cpu, std::ostream& out) {
    char buf[64];
    out << "Registers:\n";
    for (int i = 0; i < 32; i += 2) {
        // Left column
        std::snprintf(buf, sizeof(buf), "  x%-2d (%-4s) = 0x%08X",
                      i, ABI_NAMES[i], cpu.get_reg(i));
        out << buf;
        // Right column
        std::snprintf(buf, sizeof(buf), "    x%-2d (%-4s) = 0x%08X\n",
                      i + 1, ABI_NAMES[i + 1], cpu.get_reg(i + 1));
        out << buf;
    }
}

// Returns the assembly-like operand string for one instruction.
static std::string format_operands(const DecodedInstruction& instr) {
    auto xreg = [](uint32_t n) { return "x" + std::to_string(n); };
    const std::string& m = instr.mnemonic;

    // R-type: rd, rs1, rs2
    if (m == "ADD" || m == "SUB" || m == "AND" || m == "OR" || m == "XOR")
        return xreg(instr.rd) + ", " + xreg(instr.rs1) + ", " + xreg(instr.rs2);

    // I-type arithmetic: rd, rs1, imm
    if (m == "ADDI" || m == "JALR")
        return xreg(instr.rd) + ", " + xreg(instr.rs1) + ", " + std::to_string(instr.imm);

    // I-type loads: rd, imm(rs1)
    if (m == "LB" || m == "LH" || m == "LW")
        return xreg(instr.rd) + ", " + std::to_string(instr.imm) + "(" + xreg(instr.rs1) + ")";

    // S-type stores: rs2, imm(rs1)
    if (m == "SB" || m == "SH" || m == "SW")
        return xreg(instr.rs2) + ", " + std::to_string(instr.imm) + "(" + xreg(instr.rs1) + ")";

    // B-type branches: rs1, rs2, imm
    if (m == "BEQ" || m == "BNE" || m == "BLT" || m == "BGE")
        return xreg(instr.rs1) + ", " + xreg(instr.rs2) + ", " + std::to_string(instr.imm);

    // JAL: rd, imm
    if (m == "JAL")
        return xreg(instr.rd) + ", " + std::to_string(instr.imm);

    return "";
}

void trace_instruction(uint32_t pc, uint32_t raw,
                       const DecodedInstruction& instr, std::ostream& out) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "PC=0x%08X  0x%08X  %-6s ",
                  pc, raw, instr.mnemonic.c_str());
    out << buf << format_operands(instr) << "\n";
}
