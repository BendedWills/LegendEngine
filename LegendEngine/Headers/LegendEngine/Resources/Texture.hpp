#pragma once
#include <LegendEngine/Resources/Resource.hpp>

namespace le
{
	class Texture : public Resource
	{
	public:
		class APIBridge
		{
		public:
			virtual ~APIBridge() {}
		};

		virtual APIBridge* GetAPIBridge() = 0;
	};
}
