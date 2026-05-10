#include "emulator/cpu.hpp"

#include <cassert>

uint32_t CPUState::get_reg(int index) const {
    assert(index >= 0 && index < 32);
    return regs[index];
}

void CPUState::set_reg(int index, uint32_t value) {
    assert(index >= 0 && index < 32);
    if (index == 0) return;  // x0 is hardwired to zero
    regs[index] = value;
}

void CPUState::reset() {
    regs.fill(0);
    pc = 0;
}
