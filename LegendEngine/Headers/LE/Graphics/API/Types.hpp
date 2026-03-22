#pragma once

#include <cstdint>

namespace le
{
    struct Offset3D
    {
        int32_t x;
        int32_t y;
        int32_t z;
    };

    struct Extent3D
    {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
    };

    enum class PipelineStage
    {
        TOP_OF_PIPE,
        TRANSFER,
        FRAGMENT_SHADER,
    };

    enum class AccessFlagBits
    {
        TRANSFER_WRITE_BIT = 0x00001000,
        SHADER_READ_BIT = 0x00000020,
    };
}