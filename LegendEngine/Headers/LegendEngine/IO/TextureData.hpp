#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <cstdint>
#include <future>

namespace le
{
    class TextureData final
    {
    public:
        TextureData(uint64_t width, uint64_t height, uint8_t channels,
            uint8_t* data, bool shouldFree = true);
        TextureData(TextureData&&);
        ~TextureData();

        uint64_t GetWidth() const;
        uint64_t GetHeight() const;
        uint8_t GetChannels() const;
        uint8_t* GetData() const;
        bool HasLoaded() const;

        static TextureData FromData(uint64_t width, uint64_t height, uint8_t channels,
            uint8_t* data);
        static std::future<TextureData> FromFile(std::string_view path);
    private:
        uint64_t m_Width = 0;
        uint64_t m_Height = 0;
        uint8_t m_Channels = 0;
        uint8_t* m_Data = nullptr;

        bool m_ShouldFree = false;
    };
}
