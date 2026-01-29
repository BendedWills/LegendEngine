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

		explicit Logger(std::string_view name, bool enabled = true,
			bool debug = false);

		void Log(Level level, std::string_view message) const;
		void SetEnabled(bool enabled);
		void SetDebug(bool debug);

		bool IsEnabled() const;
		bool IsDebugEnabled() const;
	private:
		static std::string GetFormattedTime();

		std::string m_Name;
		bool m_Enabled;
		bool m_Debug;
	};
}