#include <gtest/gtest.h>

#include "emulator/decoder.hpp"

// All field-extraction tests use the 32-bit encoding of: ADD x2, x5, x6
//
//   bits [31:25] funct7 = 0000000
//   bits [24:20] rs2    = 00110  (x6)
//   bits [19:15] rs1    = 00101  (x5)
//   bits [14:12] funct3 = 000
//   bits [11:7]  rd     = 00010  (x2)
//   bits  [6:0]  opcode = 0110011 (0x33, R-type ALU)
//
//   Full encoding: 0x00628133
static constexpr uint32_t ADD_X2_X5_X6 = 0x00628133;

TEST(DecoderTest, ExtractOpcode) {
    EXPECT_EQ(getBits(ADD_X2_X5_X6, 0, 7), 0x33u);  // 0b0110011
}

TEST(DecoderTest, ExtractRd) {
    EXPECT_EQ(getBits(ADD_X2_X5_X6, 7, 5), 2u);  // x2
}

TEST(DecoderTest, ExtractFunct3) {
    EXPECT_EQ(getBits(ADD_X2_X5_X6, 12, 3), 0u);
}

TEST(DecoderTest, ExtractRs1) {
    EXPECT_EQ(getBits(ADD_X2_X5_X6, 15, 5), 5u);  // x5
}

TEST(DecoderTest, ExtractFunct7) {
    EXPECT_EQ(getBits(ADD_X2_X5_X6, 25, 7), 0u);
}

// 0x7FF is the largest positive 12-bit signed value (bit 11 is 0).
TEST(DecoderTest, SignExtendPositive12Bit) {
    EXPECT_EQ(signExtend(0x7FF, 12), 2047);
}

// 0x800 has bit 11 set, making it -2048 in 12-bit two's complement.
TEST(DecoderTest, SignExtendNegative12Bit) {
    EXPECT_EQ(signExtend(0x800, 12), -2048);
}
