#include <LegendEngine/IO/TextureLoader.hpp>

#include <stb_image.h>

namespace LegendEngine::IO
{
    TextureLoader::~TextureLoader()
    {
        if (m_StbIData)
            stbi_image_free(m_StbIData);
    }

    void TextureLoader::FromData(const uint64_t width,
        const uint64_t height, const uint8_t channels, uint8_t* data)
    {
        if (m_HasLoaded)
            return;

        m_Width = width;
        m_Height = height;
        m_Channels = channels;
        m_Data = data;
        m_HasLoaded = true;
    }

    void TextureLoader::FromFile(const std::string_view path)
    {
        if (m_HasLoaded)
            return;

        int width, height, channels;
        uint8_t* data = stbi_load(path.data(), &width, &height, &channels, STBI_rgb_alpha);
        if (!data)
            throw std::runtime_error("Failed to load image " + std::string(path));

        // I believe stb_image always structures the data for 4 channels
        channels = 4;

        FromData(width, height, channels, data);
    }

    uint64_t TextureLoader::GetWidth() const
    {
        return m_Width;
    }

    uint64_t TextureLoader::GetHeight() const
    {
        return m_Height;
    }

    uint8_t TextureLoader::GetChannels() const
    {
        return m_Width;
    }

    uint8_t* TextureLoader::GetData() const
    {
        return m_Data ? m_Data : m_StbIData;
    }

    bool TextureLoader::HasLoaded() const
    {
        return m_HasLoaded;
    }
}
