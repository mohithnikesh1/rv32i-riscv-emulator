#include "emulator/executor.hpp"

#include <stdexcept>

void execute(const DecodedInstruction& instr, CPUState& cpu) {
    uint32_t rs1 = cpu.get_reg(instr.rs1);
    uint32_t rs2 = cpu.get_reg(instr.rs2);
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
    } else {
        throw std::runtime_error("execute: unrecognized instruction: " + instr.mnemonic);
    }

    cpu.set_reg(instr.rd, result);
    cpu.pc += 4;
}
