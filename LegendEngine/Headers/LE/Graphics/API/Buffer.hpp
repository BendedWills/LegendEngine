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

        virtual void Update(size_t size, const void* data) = 0;
        virtual void Resize(size_t newSize) = 0;
        virtual size_t GetSize() = 0;

        virtual void* GetMappedData() { return nullptr; }
    };
}