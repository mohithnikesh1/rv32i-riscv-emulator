#pragma once

#include <istream>
#include <ostream>

#include "emulator/cpu.hpp"
#include "emulator/memory.hpp"

// Executes one instruction at cpu.pc and prints a trace line to `out`.
// Returns true if execution should continue, false if an error occurred
// (e.g. out-of-bounds PC or unrecognized instruction).
bool debugger_step(CPUState& cpu, Memory& mem, std::ostream& out);

// Prints `count` bytes of memory starting at `addr` to `out`.
// Output format: 0x00000000: AB CD EF ...  (up to 16 bytes per line)
void debugger_print_memory(const Memory& mem, uint32_t addr, uint32_t count,
                            std::ostream& out);

// Runs the interactive debugger REPL.
// Reads commands from `in` and writes all output to `out`.
// Supported commands: help, step, regs, mem <addr> <count>, run, quit.
// Returns when the user types `quit` or `in` reaches EOF.
void run_debugger(CPUState& cpu, Memory& mem, std::istream& in, std::ostream& out);
