#pragma once

#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <vector>

// Models the flat byte-addressable memory of the emulated machine.
// All multi-byte accesses use little-endian byte order (RISC-V standard).
// Out-of-bounds accesses throw std::out_of_range.
class Memory {
public:
    explicit Memory(size_t size);

    uint8_t  read8 (uint32_t addr) const;
    uint16_t read16(uint32_t addr) const;
    uint32_t read32(uint32_t addr) const;

    void write8 (uint32_t addr, uint8_t  value);
    void write16(uint32_t addr, uint16_t value);
    void write32(uint32_t addr, uint32_t value);

    size_t size() const { return data_.size(); }

private:
    std::vector<uint8_t> data_;

    void check_bounds(uint32_t addr, size_t access_size) const;
};
