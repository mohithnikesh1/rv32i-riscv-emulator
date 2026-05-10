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
static constexpr uint32_t OPCODE_OP_IMM = 0x13;  // I-type arithmetic (ADDI, ...)
static constexpr uint32_t OPCODE_OP     = 0x33;  // R-type arithmetic (ADD, SUB, AND, OR, XOR)

DecodedInstruction decode(uint32_t instruction) {
    DecodedInstruction d;

    // Fields present in every instruction format
    d.opcode = getBits(instruction,  0, 7);
    d.rd     = getBits(instruction,  7, 5);
    d.funct3 = getBits(instruction, 12, 3);
    d.rs1    = getBits(instruction, 15, 5);

    // R-type fields (bits overlap with the I-type immediate)
    d.rs2    = getBits(instruction, 20, 5);
    d.funct7 = getBits(instruction, 25, 7);

    // I-type immediate: bits [31:20], sign-extended to 32 bits
    d.imm = signExtend(getBits(instruction, 20, 12), 12);

    // --- I-type ---
    if (d.opcode == OPCODE_OP_IMM && d.funct3 == 0x0) {
        d.mnemonic = "ADDI";

    // --- R-type ---
    } else if (d.opcode == OPCODE_OP) {
        if      (d.funct3 == 0x0 && d.funct7 == 0x00) d.mnemonic = "ADD";
        else if (d.funct3 == 0x0 && d.funct7 == 0x20) d.mnemonic = "SUB";
        else if (d.funct3 == 0x7 && d.funct7 == 0x00) d.mnemonic = "AND";
        else if (d.funct3 == 0x6 && d.funct7 == 0x00) d.mnemonic = "OR";
        else if (d.funct3 == 0x4 && d.funct7 == 0x00) d.mnemonic = "XOR";
        else                                            d.mnemonic = "UNKNOWN";

    } else {
        d.mnemonic = "UNKNOWN";
    }

    return d;
}
