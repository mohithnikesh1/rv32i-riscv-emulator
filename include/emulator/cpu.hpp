#pragma once

#include <array>
#include <cstdint>

// Holds the full register state of the RV32I CPU.
struct CPUState {
    std::array<uint32_t, 32> regs = {};  // x0–x31, all start at zero
    uint32_t pc = 0;                     // program counter

    // Returns the value of register index (0–31).
    uint32_t get_reg(int index) const;

    // Sets register index to value. Writes to x0 are silently ignored.
    void set_reg(int index, uint32_t value);

    // Resets all registers and PC back to zero.
    void reset();
};
