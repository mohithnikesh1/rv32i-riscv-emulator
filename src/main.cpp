#include <iostream>

#include "emulator/cpu.hpp"
#include "emulator/decoder.hpp"
#include "emulator/executor.hpp"
#include "emulator/memory.hpp"
#include "emulator/trace.hpp"

int main() {
    CPUState cpu;
    Memory mem(1024 * 1024);  // 1 MB

    std::cout << "RISC-V RV32I Emulator\n\n";

    // A tiny program: ADDI x1, x0, 10 / ADDI x2, x0, 20 / ADD x3, x1, x2
    static constexpr uint32_t program[] = {
        0x00A00093u,  // ADDI x1, x0, 10
        0x01400113u,  // ADDI x2, x0, 20
        0x002081B3u,  // ADD  x3, x1, x2
    };

    // Load into memory at address 0
    for (uint32_t i = 0; i < 3; ++i)
        mem.write32(i * 4, program[i]);

    // Execute with trace output
    std::cout << "--- Trace ---\n";
    while (cpu.pc < 12) {
        uint32_t raw   = mem.read32(cpu.pc);
        DecodedInstruction instr = decode(raw);
        trace_instruction(cpu.pc, raw, instr, std::cout);
        execute(instr, cpu, mem);
    }

    // Dump final register state
    std::cout << "\n--- Register Dump ---\n";
    dump_registers(cpu, std::cout);

    return 0;
}
