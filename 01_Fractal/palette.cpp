#include <vector>
#include <cstdint>


uint32_t _bswap32(uint32_t a)
{
    return ((a & 0x000000FF) << 24) |
           ((a & 0x0000FF00) << 8) |
           ((a & 0x00FF0000) >> 8) |
           ((a & 0xFF000000) >> 24);
}

std::vector<uint32_t> color_ramp = {
_bswap32(0xFF1010FF),
_bswap32(0xF31017FF),
_bswap32(0xE8101EFF),
_bswap32(0xDC1126FF),
_bswap32(0xD1112DFF),
_bswap32(0xC51235FF),
_bswap32(0xBA123CFF),
_bswap32(0xAE1343FF),
_bswap32(0xA3134BFF),
_bswap32(0x971452FF),
_bswap32(0x8C145AFF),
_bswap32(0x801461FF),
_bswap32(0x751568FF),
_bswap32(0x691570FF),
_bswap32(0x5E1677FF),
_bswap32(0x52167FFF),
_bswap32(0x471786FF),
_bswap32(0x3B178DFF),
_bswap32(0x301895FF),
_bswap32(0x24189CFF),
_bswap32(0x1919A4FF)
};
