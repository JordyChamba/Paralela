#include "palette.h"

uint32_t bswap32(uint32_t a)
{
    return ((a & 0x000000FF) << 24) |
           ((a & 0x0000FF00) << 8) |
           ((a & 0x00FF0000) >> 8) |
           ((a & 0xFF000000) >> 24);
}

std::vector<uint32_t> color_ramp = {
    bswap32(0xFFFFCCFF),
    bswap32(0xFFF5B5FF),
    bswap32(0xFFEC9DFF),
    bswap32(0xFEE187FF),
    bswap32(0xFED470FF),
    bswap32(0xFEBF5AFF),
    bswap32(0xFEAB49FF),
    bswap32(0xFD9740FF),
    bswap32(0xFD7C37FF),
    bswap32(0xFC5B2EFF),
    bswap32(0xF43D25FF),
    bswap32(0xE6211EFF),
    bswap32(0xD41020FF),
    bswap32(0xC00225FF),
    bswap32(0xA10026FF),
    bswap32(0x800026FF)};

std::vector<uint32_t> color_ramp_2 = {
    bswap32(0xE9F8EDFF),
    bswap32(0xDBF3DFFF),
    bswap32(0xCCEDD2FF),
    bswap32(0xBEE8C4FF),
    bswap32(0xAFE2B5FF),
    bswap32(0x9FD9A3FF),
    bswap32(0x8ED190FF),
    bswap32(0x7EC87DFF),
    bswap32(0x71C06BFF),
    bswap32(0x68B759FF),
    bswap32(0x5FAE47FF),
    bswap32(0x56A535FF),
    bswap32(0x4C9827FF),
    bswap32(0x418A1AFF),
    bswap32(0x377B0DFF),
    bswap32(0x2C6D00FF)};

std::vector<uint32_t> color_ramp_3 = {
    bswap32(0xF5E0FFFF),
    bswap32(0xE9C6FFFF),
    bswap32(0xDDACFFFF),
    bswap32(0xD192FFFF),
    bswap32(0xC578FFFF),
    bswap32(0xB95EFFFF),
    bswap32(0xAD44FFFF),
    bswap32(0xA12AFFFF),
    bswap32(0x9510FFFF),
    bswap32(0x8900FFFF),
    bswap32(0x7F00F3FF),
    bswap32(0x7500D7FF),
    bswap32(0x6B00BBFF),
    bswap32(0x61009FFF),
    bswap32(0x570083FF),
    bswap32(0x4D0067FF)};