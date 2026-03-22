#pragma once

#include <cstdint>

namespace le
{
    class Buffer
    {
    public:
        enum class Usage
        {
            TRANSFER_SRC,
            UNIFORM_BUFFER,
            VERTEX_BUFFER,
            INDEX_BUFFER
        };

        virtual ~Buffer() = default;
        virtual void* GetMappedData() = 0;
        virtual void* GetMappedDataForFrame(size_t currentFrame) = 0;
    };
}