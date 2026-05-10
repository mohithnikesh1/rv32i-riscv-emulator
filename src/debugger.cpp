#include "emulator/debugger.hpp"

#include <cstdio>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "emulator/decoder.hpp"
#include "emulator/executor.hpp"
#include "emulator/trace.hpp"

bool debugger_step(CPUState& cpu, Memory& mem, std::ostream& out) {
    try {
        uint32_t raw = mem.read32(cpu.pc);
        DecodedInstruction instr = decode(raw);
        trace_instruction(cpu.pc, raw, instr, out);
        execute(instr, cpu, mem);
        return true;
    } catch (const std::exception& e) {
        out << "Error: " << e.what() << "\n";
        return false;
    }
}

void debugger_print_memory(const Memory& mem, uint32_t addr, uint32_t count,
                            std::ostream& out) {
    char buf[16];
    for (uint32_t i = 0; i < count; ) {
        std::snprintf(buf, sizeof(buf), "0x%08X:", addr + i);
        out << buf;
        uint32_t row_end = i + 16 < count ? i + 16 : count;
        for (uint32_t j = i; j < row_end; ++j) {
            try {
                uint8_t byte = mem.read8(addr + j);
                std::snprintf(buf, sizeof(buf), " %02X", byte);
                out << buf;
            } catch (...) {
                out << " ??";
            }
        }
        out << "\n";
        i = row_end;
    }
}

static void print_help(std::ostream& out) {
    out << "Commands:\n"
        << "  help              Print this help message\n"
        << "  step              Execute one instruction and print trace\n"
        << "  regs              Print all 32 registers\n"
        << "  mem <addr> <n>    Print n bytes of memory starting at addr\n"
        << "  run               Run until halt or error\n"
        << "  quit              Exit the debugger\n";
}

void run_debugger(CPUState& cpu, Memory& mem, std::istream& in, std::ostream& out) {
    out << "RISC-V Debugger. Type 'help' for commands.\n";
    std::string line;
    while (true) {
        out << "(riscv-dbg) " << std::flush;
        if (!std::getline(in, line)) break;

        std::istringstream tokens(line);
        std::string cmd;
        tokens >> cmd;

        if (cmd == "quit") {
            break;
        } else if (cmd == "help") {
            print_help(out);
        } else if (cmd == "step") {
            debugger_step(cpu, mem, out);
        } else if (cmd == "regs") {
            dump_registers(cpu, out);
        } else if (cmd == "run") {
            while (debugger_step(cpu, mem, out)) {}
        } else if (cmd == "mem") {
            std::string tok_addr, tok_count;
            tokens >> tok_addr >> tok_count;
            if (tok_addr.empty() || tok_count.empty()) {
                out << "Usage: mem <address> <count>\n";
            } else {
                try {
                    uint32_t addr  = static_cast<uint32_t>(std::stoul(tok_addr,  nullptr, 0));
                    uint32_t count = static_cast<uint32_t>(std::stoul(tok_count, nullptr, 0));
                    debugger_print_memory(mem, addr, count, out);
                } catch (...) {
                    out << "Usage: mem <address> <count>\n";
                }
            }
        } else if (!cmd.empty()) {
            out << "Unknown command: '" << cmd << "'. Type 'help' for commands.\n";
        }
    }
}
