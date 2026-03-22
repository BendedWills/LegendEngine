#pragma once

#include <LE/Graphics/API/Image.hpp>

namespace le
{
    class ImageView
    {
    public:
        enum class Type
        {
            TYPE_2D,
            TYPE_2D_ARRAY,
        };

        struct Info
        {
            Image* image = nullptr;
            Image::Format format = Image::Format::R8G8B8A8;
            Type type = Type::TYPE_2D;
            size_t arrayLayers = 1;
        };

        virtual ~ImageView() = default;
    };
}
