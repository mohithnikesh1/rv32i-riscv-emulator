#include "emulator/memory.hpp"

Memory::Memory(size_t size) : data_(size, 0) {}

void Memory::check_bounds(uint32_t addr, size_t access_size) const {
    if (static_cast<size_t>(addr) + access_size > data_.size()) {
        throw std::out_of_range("memory access out of bounds");
    }
}

uint8_t Memory::read8(uint32_t addr) const {
    check_bounds(addr, 1);
    return data_[addr];
}

uint16_t Memory::read16(uint32_t addr) const {
    check_bounds(addr, 2);
    return static_cast<uint16_t>(data_[addr])
         | static_cast<uint16_t>(data_[addr + 1]) << 8;
}

uint32_t Memory::read32(uint32_t addr) const {
    check_bounds(addr, 4);
    return static_cast<uint32_t>(data_[addr])
         | static_cast<uint32_t>(data_[addr + 1]) << 8
         | static_cast<uint32_t>(data_[addr + 2]) << 16
         | static_cast<uint32_t>(data_[addr + 3]) << 24;
}

void Memory::write8(uint32_t addr, uint8_t value) {
    check_bounds(addr, 1);
    data_[addr] = value;
}

void Memory::write16(uint32_t addr, uint16_t value) {
    check_bounds(addr, 2);
    data_[addr]     = static_cast<uint8_t>(value);
    data_[addr + 1] = static_cast<uint8_t>(value >> 8);
}

void Memory::write32(uint32_t addr, uint32_t value) {
    check_bounds(addr, 4);
    data_[addr]     = static_cast<uint8_t>(value);
    data_[addr + 1] = static_cast<uint8_t>(value >> 8);
    data_[addr + 2] = static_cast<uint8_t>(value >> 16);
    data_[addr + 3] = static_cast<uint8_t>(value >> 24);
}
