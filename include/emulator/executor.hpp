#pragma once

#include "emulator/cpu.hpp"
#include "emulator/decoder.hpp"
#include "emulator/memory.hpp"

// Applies the effect of a decoded instruction to the CPU state and memory.
// Advances PC by 4 on success.
// Throws std::runtime_error for unrecognized instructions.
// Throws std::out_of_range for out-of-bounds memory accesses.
void execute(const DecodedInstruction& instr, CPUState& cpu, Memory& mem);
