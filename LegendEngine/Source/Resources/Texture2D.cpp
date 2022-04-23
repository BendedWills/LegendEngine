#include <LegendEngine/Resources/Texture2D.hpp>
#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

#include <stdio.h>
#include <stb_image.h>

using namespace LegendEngine::Resources;

ITexture2DNative::ITexture2DNative(Texture2D* pTexture)
	:
	pTexture(pTexture)
{}

bool Texture2D::Init(const std::string& path)
{
	if (initialized)
		return false;
	OnInit();

	int width, height, channels;
	uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		pApplication->Log("Failed to load texture (path = \"" + path + "\")", 
			LogType::ERROR);
		return false;
	}

	if (!Init(static_cast<uint64_t>(width), static_cast<uint64_t>(height),
		static_cast<uint32_t>(channels),
		data))
	{
		pApplication->Log(
			"Failed to load texture. Native returned false. (path = \"" + path
			+ "\")",
			LogType::ERROR
		);

		stbi_image_free(data);
		return false;
	}

	stbi_image_free(data);

	initialized = true;
	return true;
}

bool Texture2D::Init(uint64_t width, uint64_t height, uint32_t channels,
	uint8_t* data)
{
	if (initialized)
		return false;

	this->width = width;
	this->height = height;
	this->channels = channels;

	if (nativeSet)
		if (!native->OnCreate(width, height, channels, data))
		{
			stbi_image_free(data);
			return false;
		}

	initialized = true;
	return true;
}

uint64_t Texture2D::GetWidth()
{
	return width;
}

uint64_t Texture2D::GetHeight()
{
	return height;
}

uint32_t Texture2D::GetChannels()
{
	return channels;
}

void Texture2D::OnResourceDispose()
{
	native->OnDispose();
}