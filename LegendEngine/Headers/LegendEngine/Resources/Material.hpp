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
		struct MaterialUniforms
		{
			float brightness = 1.0f;
		};

		Material() = default;
		LEGENDENGINE_DISPOSE_ON_DESTRUCT(Material);
		LEGENDENGINE_NO_COPY(Material);

		bool Init();

		void SetTexture(Texture2D* pTexture);
		void SetBrightness(float brightness);
		Texture2D* GetTexture();

		MaterialUniforms* GetUniforms();

		void Update();
	private:
		Texture2D* pTexture = nullptr;
		MaterialUniforms uniforms;

		bool shouldUpdateUniforms = false;

		void OnResourceDispose();
	};
}

#endif //_LEGENDENGINE_RESOURCES_MATERIAL_HPP