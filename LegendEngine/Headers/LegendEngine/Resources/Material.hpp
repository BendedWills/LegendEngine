#pragma once

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Resources/Texture2D.hpp>
#include <LegendEngine/Math/Types.hpp>

#include <memory>

namespace LegendEngine::Resources
{
	class Material : public Resource
	{
	public:
		Material() = default;
		LEGENDENGINE_NO_COPY(Material);

		void SetTexture(Texture2D* toSet);
		void SetColor(const Color& toSet);
		[[nodiscard]] Texture2D* GetTexture() const;
		[[nodiscard]] Color GetColor() const;

		void SetChanged(bool changed);
		bool HasChanged() const;
	    virtual void UpdateMaterial() = 0;

		static Scope<Material> Create();
	private:
		Texture2D* m_pTexture = nullptr;
		Color m_Color = Color(1.0f);

		bool m_Changed = false;
	};
}
