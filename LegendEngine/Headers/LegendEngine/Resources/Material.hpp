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

		virtual void OnUpdate() {}
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

		Material();
		~Material();
		LEGENDENGINE_NO_COPY(Material);

		void SetTexture(Texture2D* pTexture);
		void SetBrightness(float brightness);
		Texture2D* GetTexture();

		MaterialUniforms* GetUniforms();

		void Update();
	private:
		Texture2D* pTexture = nullptr;
		MaterialUniforms uniforms;

		bool shouldUpdateUniforms = false;
	};
}

#endif //_LEGENDENGINE_RESOURCES_MATERIAL_HPP