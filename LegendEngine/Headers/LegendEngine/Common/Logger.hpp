#pragma once

#include <string>

namespace LegendEngine
{
	class Logger
	{
	public:
		enum class Level
		{
			DEBUG,
			INFO,
			WARN,
			ERROR
		};

		Logger(std::string_view name);

		void Log(Level level, std::string_view message);
	private:
		std::string GetFormattedTime();

		std::string m_Name;
	};
}