#pragma once

#include <sstream>
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

		[[nodiscard]] bool IsEnabled() const;
		[[nodiscard]] bool IsDebugEnabled() const;

	    template <typename... Args>
	    void LogMany(const Level level, Args&&... args)
	    {
	        std::ostringstream oss;
	        ([&]
	        {
	            oss << args;
	        } (), ...);

	        Log(level, oss.str());
	    }
	private:
		static std::string GetFormattedTime();

		std::string m_Name;
		bool m_Enabled;
		bool m_Debug;
	};
}