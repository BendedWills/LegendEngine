#ifndef _LEGENDENGINE_TEXTURE2D_HPP
#define _LEGENDENGINE_TEXTURE2D_HPP

#include <LegendEngine/Common/NativeHolder.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Resources/IResource.hpp>

#include <string>

namespace LegendEngine::Resources
{
	class Texture2D;
	class ITexture2DNative
	{
	public:
		LEGENDENGINE_NO_COPY(ITexture2DNative);

		ITexture2DNative(Texture2D* pTexture);

		virtual bool OnCreate(uint64_t width, uint64_t height, uint32_t channels, 
			uint8_t* data)
		{ return false; }
		virtual void OnDispose() {}
	protected:
		Texture2D* pTexture;
	};

	class Texture2D : 
		public IResource,
		public NativeHolder<ITexture2DNative>
	{
	public:
		Texture2D() = default;
		LEGENDENGINE_DISPOSE_ON_DESTRUCT(Texture2D);
		LEGENDENGINE_NO_COPY(Texture2D);

		bool Init(const std::string& path);
		bool Init(uint64_t width, uint64_t height, uint32_t channels,
			uint8_t* data);

		uint64_t GetWidth();
		uint64_t GetHeight();
		uint32_t GetChannels();
	private:
		uint64_t width;
		uint64_t height;
		uint32_t channels;

		void OnResourceDispose();
	};
}

#endif //_LEGENDENGINE_TEXTURE2D_HPP