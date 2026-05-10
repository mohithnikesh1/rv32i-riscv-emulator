#pragma once

#include <cstdint>

// Extracts 'length' bits from 'value' starting at bit position 'start'.
// Example: getBits(0b11010, 1, 3) extracts bits [3:1] -> 0b101 = 5
uint32_t getBits(uint32_t value, int start, int length);

// Sign-extends a value that is 'bitWidth' bits wide to a full 32-bit signed integer.
// Example: signExtend(0x800, 12) -> -2048 (the 12-bit value 0x800 is negative)
int32_t signExtend(uint32_t value, int bitWidth);
