#include "emulator/decoder.hpp"

uint32_t getBits(uint32_t value, int start, int length) {
    uint32_t mask = (1u << length) - 1;
    return (value >> start) & mask;
}

int32_t signExtend(uint32_t value, int bitWidth) {
    uint32_t sign_bit = 1u << (bitWidth - 1);
    if (value & sign_bit) {
        // The value is negative in 'bitWidth'-bit two's complement.
        // Fill all bits above position (bitWidth-1) with 1s.
        uint32_t fill = ~((1u << bitWidth) - 1);
        return static_cast<int32_t>(value | fill);
    }
    return static_cast<int32_t>(value);
}
