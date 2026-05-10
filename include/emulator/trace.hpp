#pragma once

#include <cstdint>
#include <iosfwd>

#include "emulator/cpu.hpp"
#include "emulator/decoder.hpp"

// Prints all 32 registers in a two-column table with ABI names.
// Example line:  x1  (ra)  = 0x00000004    x2  (sp)  = 0x00000000
void dump_registers(const CPUState& cpu, std::ostream& out);

// Prints a single trace line for one instruction, before it is executed.
// Includes the PC at the time of execution, the raw 32-bit word, the mnemonic,
// and operands formatted in assembly-like syntax.
// Example:  PC=0x00000000  0x00500093  ADDI   x1, x0, 5
void trace_instruction(uint32_t pc, uint32_t raw,
                       const DecodedInstruction& instr, std::ostream& out);
