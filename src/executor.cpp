#include "emulator/executor.hpp"

#include <stdexcept>

void execute(const DecodedInstruction& instr, CPUState& cpu, Memory& mem) {
    uint32_t rs1 = cpu.get_reg(instr.rs1);
    uint32_t rs2 = cpu.get_reg(instr.rs2);

    // --- Branches: no register written, no memory access ---
    // If condition is true, PC jumps to PC + imm; otherwise PC advances by 4.
    if (instr.mnemonic == "BEQ" || instr.mnemonic == "BNE" ||
        instr.mnemonic == "BLT" || instr.mnemonic == "BGE") {

        int32_t  s1 = static_cast<int32_t>(rs1);
        int32_t  s2 = static_cast<int32_t>(rs2);
        bool taken = false;

        if      (instr.mnemonic == "BEQ") taken = (rs1 == rs2);
        else if (instr.mnemonic == "BNE") taken = (rs1 != rs2);
        else if (instr.mnemonic == "BLT") taken = (s1  <  s2);
        else if (instr.mnemonic == "BGE") taken = (s1  >= s2);

        cpu.pc += taken ? static_cast<uint32_t>(instr.imm) : 4u;
        return;
    }

    // --- Stores: write to memory, no register destination ---
    // (For S-type, instr.rd holds imm[4:0] and must not be used as a register index.)
    if (instr.mnemonic == "SB" || instr.mnemonic == "SH" || instr.mnemonic == "SW") {
        uint32_t addr = rs1 + static_cast<uint32_t>(instr.imm);
        if      (instr.mnemonic == "SB") mem.write8 (addr, static_cast<uint8_t> (rs2));
        else if (instr.mnemonic == "SH") mem.write16(addr, static_cast<uint16_t>(rs2));
        else                             mem.write32(addr, rs2);
        cpu.pc += 4;
        return;
    }

    // --- All register-writing instructions ---
    uint32_t result;

    if (instr.mnemonic == "ADDI") {
        result = rs1 + static_cast<uint32_t>(instr.imm);
    } else if (instr.mnemonic == "ADD") {
        result = rs1 + rs2;
    } else if (instr.mnemonic == "SUB") {
        result = rs1 - rs2;
    } else if (instr.mnemonic == "AND") {
        result = rs1 & rs2;
    } else if (instr.mnemonic == "OR") {
        result = rs1 | rs2;
    } else if (instr.mnemonic == "XOR") {
        result = rs1 ^ rs2;
    } else if (instr.mnemonic == "LB") {
        uint32_t addr = rs1 + static_cast<uint32_t>(instr.imm);
        result = static_cast<uint32_t>(signExtend(mem.read8(addr), 8));
    } else if (instr.mnemonic == "LH") {
        uint32_t addr = rs1 + static_cast<uint32_t>(instr.imm);
        result = static_cast<uint32_t>(signExtend(mem.read16(addr), 16));
    } else if (instr.mnemonic == "LW") {
        uint32_t addr = rs1 + static_cast<uint32_t>(instr.imm);
        result = mem.read32(addr);
    } else {
        throw std::runtime_error("execute: unrecognized instruction: " + instr.mnemonic);
    }

    cpu.set_reg(instr.rd, result);
    cpu.pc += 4;
}
