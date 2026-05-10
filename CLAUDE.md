# CLAUDE.md

## Project Overview

This project is an RV32I RISC-V emulator written in C++20.

The goal is to build a clean, tested, CV-worthy systems programming project.

Start with RV32I only. Do not add RV64, floating point, compressed instructions, privileged mode, virtual memory, Linux booting, or multicore support unless explicitly requested later.

## Core Features

The emulator should eventually support:

- CPU state
- 32 RISC-V registers
- program counter
- memory model
- instruction fetch
- instruction decode
- instruction execution
- trace mode
- debugger
- disassembler
- automated tests
- GitHub Actions CI

## Architecture Rules

Keep these modules separate:

- CPU state
- Memory
- Decoder
- Executor
- Loader
- Debugger
- Disassembler
- CLI

The decoder must only convert raw instruction bits into decoded instruction information.

The executor must apply decoded instruction behavior to CPU state and memory.

Do not mix decoding, execution, CLI logic, and tests in one file.

## Correctness Rules

- Register x0 must always remain zero.
- PC normally increases by 4.
- Branch and jump instructions must update PC correctly.
- Sign extension must be tested carefully.
- Signed and unsigned comparisons must be handled separately.
- Memory access must check bounds.
- Invalid instructions should produce clean errors.

## Testing Rules

Every new instruction must include tests.

Tests should cover:

- normal behavior
- negative immediates where relevant
- writing to x0
- signed versus unsigned behavior
- branch taken and branch not taken
- memory bounds where relevant

## Coding Style

- Use C++20.
- Use CMake.
- Prefer simple readable code.
- Avoid clever code.
- Add comments only when the behavior is not obvious.
- Do not add extra features unless requested.

## Development Workflow

Before writing code:

1. Explain the plan.
2. List the files that will change.
3. Make a small useful change.
4. Add tests.
5. Run tests if possible.
6. Summarize what changed.

## CV Goal

This project should be suitable for a GitHub portfolio and resume. Prioritize correctness, documentation, tests, and clean architecture.