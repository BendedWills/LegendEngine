#include <LegendEngine/Resources/Texture2DArray.hpp>

namespace le
{
	Texture2DArray::Texture2DArray(size_t width, size_t height, uint8_t channels)
		:
		m_Width(width),
		m_Height(height),
		m_Channels(channels)
	{}

	size_t Texture2DArray::GetWidth() const
	{
		return m_Width;
	}

	size_t Texture2DArray::GetHeight() const
	{
		return m_Height;
	}

	uint8_t Texture2DArray::GetChannels() const
	{
		return m_Channels;
	}
}
