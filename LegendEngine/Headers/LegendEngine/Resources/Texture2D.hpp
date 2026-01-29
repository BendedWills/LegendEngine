#pragma once

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Resources/Resource.hpp>
#include <LegendEngine/IO/TextureLoader.hpp>

#include <memory>

namespace LegendEngine::Resources
{
	class Texture2D : public Resource
	{
	public:
		explicit Texture2D(const IO::TextureLoader& loader);
	    ~Texture2D() override = default;
		LEGENDENGINE_NO_COPY(Texture2D);

		uint64_t GetWidth() const;
		uint64_t GetHeight() const;
		uint8_t GetChannels() const;

		static Scope<Texture2D> Create(IO::TextureLoader& loader);
	private:
		uint64_t m_Width;
		uint64_t m_Height;
		uint8_t m_Channels;
	};
}
