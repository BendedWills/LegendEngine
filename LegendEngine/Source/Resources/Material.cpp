#include <LegendEngine/Resources/Material.hpp>

#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

namespace LegendEngine::Resources
{
	IMaterialNative::IMaterialNative(Material* pMaterial)
		:
		pMaterial(pMaterial)
	{}

	Material::Material()
	{

	}

	Material::~Material()
	{
		LEGENDENGINE_OBJECT_LOG(
			pApplication, "Material",
			"Disposed material",
			LogType::DEBUG
		);
	}

	void Material::SetTexture(Texture2D* toSet)
	{
		this->pTexture = toSet;
		shouldUpdateUniforms = true;
	}

	void Material::SetColor(const Color &toSet)
	{
		uniforms.color = toSet;
		shouldUpdateUniforms = true;
	}

	Texture2D* Material::GetTexture() const
	{
		return pTexture;
	}

	Color Material::GetColor()
	{
		return color;
	}

	Material::MaterialUniforms* Material::GetUniforms()
	{
		return &uniforms;
	}

	void Material::Update()
	{
		// The shouldUpdateUniforms variable is set to true when an attribute of the
		// material has changed and therefore must be updated on the GPU. If nothing
		// has changed since the material was last updated, return.
		if (!shouldUpdateUniforms)
			return;

		// Update the native as well (with Vulkan this updates the uniform buffer).
		if (nativeSet)
			native->OnUpdate();
		pApplication->GetRenderer().OnResourceChange(this);

		// Set this to false since the uniforms have been updated.
		shouldUpdateUniforms = false;
	}
}
