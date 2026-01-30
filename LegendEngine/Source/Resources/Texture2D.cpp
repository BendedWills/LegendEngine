#include <LegendEngine/Application.hpp>
#include <LegendEngine/Resources/Texture2D.hpp>

namespace LegendEngine::Resources
{
    Texture2D::Texture2D(const IO::TextureLoader& loader)
        :
        m_Width(loader.GetWidth()),
        m_Height(loader.GetHeight()),
        m_Channels(loader.GetChannels())
    {
        if (!loader.HasLoaded())
            throw std::runtime_error("Tried to create texture with uninitialized loader");
    }

    uint64_t Texture2D::GetWidth() const
    {
        return m_Width;
    }

    uint64_t Texture2D::GetHeight() const
    {
        return m_Height;
    }

    uint8_t Texture2D::GetChannels() const
    {
        return m_Channels;
    }

    Scope<Texture2D> Texture2D::Create(IO::TextureLoader& loader)
    {
        LGENG_DEBUG_LOG("Texture2D created");
        return Application::Get().GetGraphicsContext().CreateTexture2D(loader);
    }
}