#include "emulator/decoder.hpp"

uint32_t getBits(uint32_t value, int start, int length) {
    uint32_t mask = (1u << length) - 1;
    return (value >> start) & mask;
}

int32_t signExtend(uint32_t value, int bitWidth) {
    uint32_t sign_bit = 1u << (bitWidth - 1);
    if (value & sign_bit) {
        // The value is negative in 'bitWidth'-bit two's complement.
        // Fill all bits above position (bitWidth-1) with 1s.
        uint32_t fill = ~((1u << bitWidth) - 1);
        return static_cast<int32_t>(value | fill);
    }
    return static_cast<int32_t>(value);
}

// Opcode constants
static constexpr uint32_t OPCODE_OP_IMM = 0x13;  // I-type arithmetic (ADDI, SLTI, ...)

DecodedInstruction decode(uint32_t instruction) {
    DecodedInstruction d;

    d.opcode = getBits(instruction,  0, 7);
    d.rd     = getBits(instruction,  7, 5);
    d.funct3 = getBits(instruction, 12, 3);
    d.rs1    = getBits(instruction, 15, 5);

    // I-type immediate: bits [31:20], sign-extended to 32 bits
    d.imm = signExtend(getBits(instruction, 20, 12), 12);

    if (d.opcode == OPCODE_OP_IMM && d.funct3 == 0x0) {
        d.mnemonic = "ADDI";
    } else {
        d.mnemonic = "UNKNOWN";
    }

    return d;
}
