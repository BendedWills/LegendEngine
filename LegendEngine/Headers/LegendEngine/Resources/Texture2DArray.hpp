#pragma once
#include <LegendEngine/Resources/Texture.hpp>

namespace le
{
	class Texture2DArray : public Texture
	{
	public:
		Texture2DArray(size_t width, size_t height, uint8_t channels);

		size_t GetWidth() const;
		size_t GetHeight() const;
		uint8_t GetChannels() const;
	private:
		size_t m_Width;
		size_t m_Height;
		uint8_t m_Channels;
	};
}
