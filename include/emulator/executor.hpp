#pragma once

#include "emulator/cpu.hpp"
#include "emulator/decoder.hpp"

// Applies the effect of a decoded instruction to the CPU state.
// Advances PC by 4 on success.
// Throws std::runtime_error for unrecognized instructions.
void execute(const DecodedInstruction& instr, CPUState& cpu);
