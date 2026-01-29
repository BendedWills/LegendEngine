#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <cstdint>

namespace LegendEngine::IO
{
    class TextureLoader final
    {
    public:
        TextureLoader() = default;
        ~TextureLoader();
        LEGENDENGINE_NO_COPY(TextureLoader);

        void FromData(uint64_t width, uint64_t height, uint8_t channels,
            uint8_t* data);
        void FromFile(std::string_view path);

        uint64_t GetWidth() const;
        uint64_t GetHeight() const;
        uint8_t GetChannels() const;
        uint8_t* GetData() const;
        bool HasLoaded() const;
    private:
        uint64_t m_Width = 0;
        uint64_t m_Height = 0;
        uint8_t m_Channels = 0;
        uint8_t* m_Data = nullptr;
        uint8_t* m_StbIData = nullptr;

        bool m_HasLoaded = false;
    };
}