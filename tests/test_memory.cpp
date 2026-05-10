#include <gtest/gtest.h>

#include "emulator/memory.hpp"

TEST(MemoryTest, ConstructionSucceeds) {
    Memory mem(1024);
    EXPECT_EQ(mem.size(), 1024u);
}

TEST(MemoryTest, InitialBytesAreZero) {
    Memory mem(256);
    for (uint32_t i = 0; i < 256; i++) {
        EXPECT_EQ(mem.read8(i), 0u);
    }
}

TEST(MemoryTest, ReadWriteByte) {
    Memory mem(1024);
    mem.write8(0, 0xAB);
    EXPECT_EQ(mem.read8(0), 0xABu);
}

TEST(MemoryTest, ReadWriteHalfword) {
    Memory mem(1024);
    mem.write16(0, 0x1234);
    EXPECT_EQ(mem.read16(0), 0x1234u);
}

TEST(MemoryTest, ReadWriteWord) {
    Memory mem(1024);
    mem.write32(0, 0xDEADBEEF);
    EXPECT_EQ(mem.read32(0), 0xDEADBEEFu);
}

// RISC-V is little-endian: the low byte of a word sits at the lowest address.
TEST(MemoryTest, LittleEndianLayout) {
    Memory mem(1024);
    mem.write32(0, 0x04030201);
    EXPECT_EQ(mem.read8(0), 0x01u);
    EXPECT_EQ(mem.read8(1), 0x02u);
    EXPECT_EQ(mem.read8(2), 0x03u);
    EXPECT_EQ(mem.read8(3), 0x04u);
}

TEST(MemoryTest, OutOfBoundsReadThrows) {
    Memory mem(1024);
    EXPECT_THROW(mem.read8(1024), std::out_of_range);
}

TEST(MemoryTest, OutOfBoundsWriteThrows) {
    Memory mem(1024);
    EXPECT_THROW(mem.write8(1024, 0xFF), std::out_of_range);
}

TEST(MemoryTest, ReadWordAtEndThrows) {
    Memory mem(1024);
    // addr 1022 + 4 bytes = 1026, which is out of range
    EXPECT_THROW(mem.read32(1022), std::out_of_range);
}
