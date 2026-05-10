#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "emulator/cpu.hpp"
#include "emulator/debugger.hpp"
#include "emulator/memory.hpp"

// ---------------------------------------------------------------------------
// debugger_step
// ---------------------------------------------------------------------------

TEST(DebuggerStep, AdvancesPCAndExecutes) {
    CPUState cpu;
    Memory   mem(1024);
    mem.write32(0, 0x00500093u);  // ADDI x1, x0, 5
    std::ostringstream out;
    bool ok = debugger_step(cpu, mem, out);
    EXPECT_TRUE(ok);
    EXPECT_EQ(cpu.pc, 4u);
    EXPECT_EQ(cpu.get_reg(1), 5u);
}

TEST(DebuggerStep, PrintsTraceLine) {
    CPUState cpu;
    Memory   mem(1024);
    mem.write32(0, 0x00500093u);  // ADDI x1, x0, 5
    std::ostringstream out;
    debugger_step(cpu, mem, out);
    EXPECT_NE(out.str().find("ADDI"), std::string::npos);
    EXPECT_NE(out.str().find("PC="),  std::string::npos);
}

TEST(DebuggerStep, ReturnsFalseOnOutOfBoundsPC) {
    CPUState cpu;
    Memory   mem(4);
    cpu.pc = 100;  // far outside 4-byte memory
    std::ostringstream out;
    bool ok = debugger_step(cpu, mem, out);
    EXPECT_FALSE(ok);
    EXPECT_NE(out.str().find("Error"), std::string::npos);
}

// ---------------------------------------------------------------------------
// debugger_print_memory
// ---------------------------------------------------------------------------

TEST(DebuggerPrintMemory, ShowsAddressAndBytes) {
    Memory mem(16);
    mem.write8(0, 0xAB);
    mem.write8(1, 0xCD);
    std::ostringstream out;
    debugger_print_memory(mem, 0, 2, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("0x00000000:"), std::string::npos);
    EXPECT_NE(s.find("AB"),         std::string::npos);
    EXPECT_NE(s.find("CD"),         std::string::npos);
}

TEST(DebuggerPrintMemory, WrapsAt16BytesPerLine) {
    Memory mem(32);
    std::ostringstream out;
    debugger_print_memory(mem, 0, 32, out);
    // Two address labels should appear: 0x00000000 and 0x00000010
    const std::string s = out.str();
    EXPECT_NE(s.find("0x00000000:"), std::string::npos);
    EXPECT_NE(s.find("0x00000010:"), std::string::npos);
}

// ---------------------------------------------------------------------------
// run_debugger – REPL command dispatch
// ---------------------------------------------------------------------------

TEST(RunDebugger, QuitExitsCleanly) {
    CPUState cpu;
    Memory   mem(1024);
    std::istringstream in("quit\n");
    std::ostringstream out;
    run_debugger(cpu, mem, in, out);
    SUCCEED();  // reaching here means it returned without hanging
}

TEST(RunDebugger, HelpListsAllCommands) {
    CPUState cpu;
    Memory   mem(1024);
    std::istringstream in("help\nquit\n");
    std::ostringstream out;
    run_debugger(cpu, mem, in, out);
    const std::string s = out.str();
    EXPECT_NE(s.find("step"), std::string::npos);
    EXPECT_NE(s.find("regs"), std::string::npos);
    EXPECT_NE(s.find("mem"),  std::string::npos);
    EXPECT_NE(s.find("run"),  std::string::npos);
    EXPECT_NE(s.find("quit"), std::string::npos);
}

TEST(RunDebugger, StepExecutesOneInstruction) {
    CPUState cpu;
    Memory   mem(1024);
    mem.write32(0, 0x00500093u);  // ADDI x1, x0, 5
    std::istringstream in("step\nquit\n");
    std::ostringstream out;
    run_debugger(cpu, mem, in, out);
    EXPECT_EQ(cpu.pc,         4u);
    EXPECT_EQ(cpu.get_reg(1), 5u);
}

TEST(RunDebugger, RegsPrintsRegisterDump) {
    CPUState cpu;
    Memory   mem(1024);
    std::istringstream in("regs\nquit\n");
    std::ostringstream out;
    run_debugger(cpu, mem, in, out);
    EXPECT_NE(out.str().find("Registers:"), std::string::npos);
}

TEST(RunDebugger, MemPrintsMemoryContents) {
    CPUState cpu;
    Memory   mem(1024);
    mem.write8(0, 0x42);
    std::istringstream in("mem 0 1\nquit\n");
    std::ostringstream out;
    run_debugger(cpu, mem, in, out);
    EXPECT_NE(out.str().find("42"), std::string::npos);
}

TEST(RunDebugger, MemAcceptsHexAddress) {
    CPUState cpu;
    Memory   mem(1024);
    mem.write8(16, 0xFF);
    std::istringstream in("mem 0x10 1\nquit\n");
    std::ostringstream out;
    run_debugger(cpu, mem, in, out);
    EXPECT_NE(out.str().find("FF"), std::string::npos);
}

TEST(RunDebugger, RunExecutesMultipleInstructions) {
    CPUState cpu;
    Memory   mem(1024);
    mem.write32(0, 0x00500093u);  // ADDI x1, x0, 5
    mem.write32(4, 0x00A00113u);  // ADDI x2, x0, 10
    // address 8 onward is zero (UNKNOWN) -> run stops
    std::istringstream in("run\nquit\n");
    std::ostringstream out;
    run_debugger(cpu, mem, in, out);
    EXPECT_EQ(cpu.get_reg(1), 5u);
    EXPECT_EQ(cpu.get_reg(2), 10u);
}

TEST(RunDebugger, UnknownCommandReportsError) {
    CPUState cpu;
    Memory   mem(1024);
    std::istringstream in("foobar\nquit\n");
    std::ostringstream out;
    run_debugger(cpu, mem, in, out);
    EXPECT_NE(out.str().find("Unknown command"), std::string::npos);
}

TEST(RunDebugger, MemMissingArgsPrintsUsage) {
    CPUState cpu;
    Memory   mem(1024);
    std::istringstream in("mem\nquit\n");
    std::ostringstream out;
    run_debugger(cpu, mem, in, out);
    EXPECT_NE(out.str().find("Usage"), std::string::npos);
}
