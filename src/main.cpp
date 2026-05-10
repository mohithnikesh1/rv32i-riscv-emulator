#include <iostream>

#include "emulator/cpu.hpp"
#include "emulator/memory.hpp"

int main() {
    CPUState cpu;
    Memory mem(1024 * 1024);  // 1 MB

    std::cout << "RISC-V RV32I Emulator\n";
    std::cout << "PC:          " << cpu.pc << "\n";
    std::cout << "Memory size: " << mem.size() << " bytes\n";

    return 0;
}
