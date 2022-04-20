#include <LegendEngine/Resources/Material.hpp>

#include <LegendEngine/Application.hpp>
#include <LegendEngine/Common/Defs.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Resources;

IMaterialNative::IMaterialNative(Material* pMaterial)
	:
	pMaterial(pMaterial)
{}

bool Material::Init()
{
	if (initialized)
		return false;
	OnInit();

	if (nativeSet)
		if (!native->OnCreate())
			return false;

	initialized = true;
	return true;
}

void Material::SetTexture(Texture2D* pTexture)
{
	this->pTexture = pTexture;

	if (nativeSet)
		native->OnUpdate();
}

Texture2D* Material::GetTexture()
{
	return pTexture;
}

void Material::OnResourceDispose()
{
    if (nativeSet)
        native->OnDispose();

	LEGENDENGINE_OBJECT_LOG(
		pApplication, "Material",
		"Disposed material",
		LogType::DEBUG
	);
}