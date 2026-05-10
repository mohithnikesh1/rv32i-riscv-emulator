# RV32I RISC-V Emulator

A cycle-accurate emulator for the RV32I base integer instruction set, written in modern C++20.
Built as a clean, well-tested systems programming project.

---

## Features

- **CPU state** — 32-register file with x0 hardwired to zero and a 32-bit program counter
- **Memory** — flat byte-addressable memory with little-endian multi-byte access and bounds checking
- **Instruction decoding** — separates raw bit extraction from execution logic
- **Instruction execution** — all RV32I base integer instructions (arithmetic, loads, stores, branches, jumps)
- **Trace mode** — prints each instruction as it executes: PC, raw encoding, mnemonic, and operands
- **Register dump** — prints all 32 registers with both register number and ABI name (e.g. `x10 (a0)`)
- **Interactive debugger** — REPL with `step`, `regs`, `mem`, `run`, and `quit` commands
- **Automated tests** — 84 unit tests across all modules using GoogleTest

---

## Supported Instructions

| Format  | Instructions                         |
|---------|--------------------------------------|
| R-type  | `ADD`, `SUB`, `AND`, `OR`, `XOR`     |
| I-type  | `ADDI`                               |
| I-type  | `LB`, `LH`, `LW` (sign-extended)     |
| I-type  | `JALR`                               |
| S-type  | `SB`, `SH`, `SW`                     |
| B-type  | `BEQ`, `BNE`, `BLT`, `BGE`          |
| J-type  | `JAL`                                |

All instructions follow the RV32I specification, including correct sign extension,
signed vs. unsigned comparison, and PC-relative addressing for branches and jumps.

---

## Build Instructions

**Prerequisites:**
- CMake 3.20 or newer
- A C++20-compatible compiler: GCC 10+, Clang 11+, or MSVC 2019+
- Internet access on first build (CMake fetches GoogleTest automatically)

**Steps:**

```bash
git clone <repo-url>
cd riscv-emulator
cmake -B build -S .
cmake --build build
```

On Windows with Visual Studio you can also open the folder directly in Visual Studio or VS Code
and use the built-in CMake support.

---

## Usage

**Run the default demo** (loads a 3-instruction program, prints trace and register dump):

```bash
./build/riscv_emulator
```

Example output:

```
RISC-V RV32I Emulator

--- Trace ---
PC=0x00000000  0x00A00093  ADDI   x1, x0, 10
PC=0x00000004  0x01400113  ADDI   x2, x0, 20
PC=0x00000008  0x002081B3  ADD    x3, x1, x2

--- Register Dump ---
Registers:
  x0  (zero) = 0x00000000    x1  (ra  ) = 0x0000000A
  x2  (sp  ) = 0x00000014    x3  (gp  ) = 0x0000001E
  ...
```

**Run the interactive debugger:**

```bash
./build/riscv_emulator --debug
```

Debugger session example:

```
RISC-V Debugger. Type 'help' for commands.
(riscv-dbg) help
Commands:
  help              Print this help message
  step              Execute one instruction and print trace
  regs              Print all 32 registers
  mem <addr> <n>    Print n bytes of memory starting at addr
  run               Run until halt or error
  quit              Exit the debugger
(riscv-dbg) step
PC=0x00000000  0x00A00093  ADDI   x1, x0, 10
(riscv-dbg) regs
Registers:
  x0  (zero) = 0x00000000    x1  (ra  ) = 0x0000000A
  ...
(riscv-dbg) mem 0 8
0x00000000: 93 00 A0 00 13 01 40 01
(riscv-dbg) quit
```

---

## Testing

Build and run all tests:

```bash
cmake --build build
cd build
ctest --output-on-failure
```

Or run individual test suites directly:

```bash
./build/tests/test_cpu
./build/tests/test_memory
./build/tests/test_decoder
./build/tests/test_executor
./build/tests/test_trace
./build/tests/test_debugger
```

All 84 tests should pass. Test coverage includes:
- CPU: register initialization, x0 immutability, reset
- Memory: read/write at all widths, little-endian layout, bounds checking
- Decoder: bit field extraction, sign extension, all instruction formats
- Executor: every instruction's behavior, edge cases, x0 writes, signed comparisons
- Trace: output format for all instruction types, non-interference with CPU state
- Debugger: step, run, regs, mem, help, quit, error handling

---

## Project Roadmap

| Phase | Status     | Description                                  |
|-------|------------|----------------------------------------------|
| 1     | Complete   | CPU state, memory, RV32I decode and execute  |
| 2     | Complete   | Trace mode and register dump                 |
| 3     | Complete   | Interactive debugger (step, run, regs, mem)  |
| 4     | Planned    | Disassembler (standalone binary output)      |
| 5     | Planned    | ELF loader (run real RISC-V binaries)        |
| 6     | Planned    | GitHub Actions CI                            |
| 7     | Planned    | Breakpoints and watchpoints in debugger      |

---

## CV Bullet

> Built an RV32I RISC-V emulator in C++20 supporting full base integer instruction set execution, interactive debugging, trace mode, and 84 automated unit tests — demonstrating systems programming skills in CPU simulation, binary decoding, and clean modular architecture.

---

## Non-Goals for Version 1

The first version does not support:

- RV64 or floating-point extensions
- Compressed instructions (C extension)
- Linux booting or ELF loading (planned for phase 5)
- Virtual memory or privileged mode
- Interrupts or multicore execution
