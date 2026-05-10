#pragma once

#include <cstdint>
#include <string>

// Extracts 'length' bits from 'value' starting at bit position 'start'.
// Example: getBits(0b11010, 1, 3) extracts bits [3:1] -> 0b101 = 5
uint32_t getBits(uint32_t value, int start, int length);

// Sign-extends a value that is 'bitWidth' bits wide to a full 32-bit signed integer.
// Example: signExtend(0x800, 12) -> -2048 (the 12-bit value 0x800 is negative)
int32_t signExtend(uint32_t value, int bitWidth);

// Holds the decoded fields of one RV32I instruction.
// Only fields relevant to the recognized instruction type are meaningful.
// I-type (ADDI, loads): opcode, rd, funct3, rs1, imm
// R-type (ADD, SUB...): opcode, rd, funct3, rs1, rs2, funct7
// S-type (stores):      opcode, funct3, rs1, rs2, imm  (rd holds imm[4:0] internally)
// B-type (branches):    opcode, funct3, rs1, rs2, imm  (PC-relative byte offset)
// J-type (JAL):         opcode, rd, imm               (PC-relative byte offset, 21-bit)
struct DecodedInstruction {
    uint32_t    opcode;     // bits [6:0]
    uint32_t    rd;         // bits [11:7]  (not meaningful for stores)
    uint32_t    funct3;     // bits [14:12]
    uint32_t    rs1;        // bits [19:15]
    uint32_t    rs2;        // bits [24:20]  (R-type and S-type)
    uint32_t    funct7;     // bits [31:25]  (R-type only)
    int32_t     imm;        // sign-extended immediate (I-type and S-type)
    std::string mnemonic;   // "ADDI", "ADD", "SUB", "AND", "OR", "XOR",
                            // "LB", "LH", "LW", "SB", "SH", "SW",
                            // "BEQ", "BNE", "BLT", "BGE",
                            // "JAL", "JALR", or "UNKNOWN"
};

// Decodes a 32-bit raw instruction word into a DecodedInstruction.
DecodedInstruction decode(uint32_t instruction);
