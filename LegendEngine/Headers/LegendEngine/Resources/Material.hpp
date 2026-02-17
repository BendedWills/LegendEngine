#pragma once

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Resources/Texture2D.hpp>
#include <LegendEngine/Math/Types.hpp>

#include <memory>
#include <LegendEngine/Graphics/ShaderManager.hpp>
#include <LegendEngine/Resources/Shader.hpp>

namespace LegendEngine::Resources
{
	class Material : public Resource
	{
	public:
		Material();
		LEGENDENGINE_NO_COPY(Material);

		void SetTexture(Texture2D* toSet);
		void SetColor(const Color& toSet);
		[[nodiscard]] Texture2D* GetTexture() const;
	    [[nodiscard]] Shader* GetShader() const;
		[[nodiscard]] Color GetColor() const;

		void SetChanged(bool changed);
		bool HasChanged() const;
	    virtual void UpdateMaterial() = 0;

		static Scope<Material> Create();
	private:
	    const Graphics::ShaderManager& m_ShaderManager;

		Texture2D* m_pTexture = nullptr;
	    Shader* m_pShader;
		Color m_Color = Color(1.0f);

		bool m_Changed = false;
	};
}
