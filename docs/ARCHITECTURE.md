# Architecture

## CPUState

Stores:

- 32 registers
- program counter
- running/stopped state

Register x0 must always be zero.

## Memory

Stores program and data bytes.

Must support:

- read8
- read16
- read32
- write8
- write16
- write32

## Decoder

Converts raw 32-bit instruction bits into decoded instruction fields.

## Executor

Executes decoded instructions and updates CPU/memory state.

## CLI

Allows the user to run the emulator from the terminal.

## Tests

Verify each part of the emulator.