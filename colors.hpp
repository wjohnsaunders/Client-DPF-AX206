#pragma once

#include <cstdint>

// The DPF screen uses RGB565 format, but the bytes are big-endian.
// Use the GCC builtin function __builtin_bswap16 to convert to big-endian.

constexpr uint16_t RGB565(int red, int green, int blue)
{
    return __builtin_bswap16(((red << 8) & 0xf800) | ((green << 3) & 0x7e0) | ((blue >> 3) & 0x001f));
}

constexpr uint16_t colorBlack = RGB565(0, 0, 0);
constexpr uint16_t colorGrey10 = RGB565(25, 25, 25);
constexpr uint16_t colorGrey20 = RGB565(51, 51, 51);
constexpr uint16_t colorGrey30 = RGB565(76, 76, 76);
constexpr uint16_t colorGrey40 = RGB565(102, 102, 102);
constexpr uint16_t colorGrey50 = RGB565(127, 127, 127);
constexpr uint16_t colorWhite = RGB565(255, 255, 255);
constexpr uint16_t colorRed = RGB565(255, 0, 0);
constexpr uint16_t colorGreen = RGB565(0, 255, 0);
constexpr uint16_t colorBlue = RGB565(0, 0, 255);
constexpr uint16_t colorYellow = RGB565(255, 255, 0);
constexpr uint16_t colorMagenta = RGB565(255, 0, 255);
constexpr uint16_t colorCyan = RGB565(0, 255, 255);
