#include "emulator/executor.hpp"

#include <stdexcept>

void execute(const DecodedInstruction& instr, CPUState& cpu) {
    if (instr.mnemonic == "ADDI") {
        // rd = rs1 + sign_extended_immediate (wraps on overflow, per RISC-V spec)
        uint32_t result = cpu.get_reg(instr.rs1) + static_cast<uint32_t>(instr.imm);
        cpu.set_reg(instr.rd, result);
        cpu.pc += 4;
        return;
    }

    throw std::runtime_error("execute: unrecognized instruction: " + instr.mnemonic);
}
