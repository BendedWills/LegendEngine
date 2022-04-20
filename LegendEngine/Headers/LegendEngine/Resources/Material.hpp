#ifndef _LEGENDENGINE_RESOURCES_MATERIAL_HPP
#define _LEGENDENGINE_RESOURCES_MATERIAL_HPP

#include <LegendEngine/Common/NativeHolder.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Resources/IResource.hpp>
#include <LegendEngine/Resources/Texture2D.hpp>

#include <string>

namespace LegendEngine::Resources
{
	class Material;
	class IMaterialNative
	{
	public:
		LEGENDENGINE_NO_COPY(IMaterialNative);

		IMaterialNative(Material* pMaterial);

		virtual bool OnCreate()
		{ return false; }

		virtual void OnUpdate() {}

		virtual void OnDispose() {}
	protected:
		Material* pMaterial;
	};

	class Material : 
		public IResource,
		public NativeHolder<IMaterialNative>
	{
	public:
		Material() = default;
		LEGENDENGINE_DISPOSE_ON_DESTRUCT(Material);
		LEGENDENGINE_NO_COPY(Material);

		bool Init();

		void SetTexture(Texture2D* pTexture);
		Texture2D* GetTexture();
	private:
		Texture2D* pTexture;

		void OnResourceDispose();
	};
}

#endif //_LEGENDENGINE_RESOURCES_MATERIAL_HPP