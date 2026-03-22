#pragma once

#include <LE/Graphics/API/Image.hpp>
#include <LE/Graphics/API/ImageView.hpp>
#include <LE/Resources/Resource.hpp>

namespace le
{
	class Texture : public Resource
	{
	public:
		virtual Image& GetImage() = 0;
		virtual ImageView& GetImageView() = 0;

		ID<Texture> id = ID<Texture>(m_uid);
	};
}
