#include <LegendEngine/Common/Logger.hpp>

#include <ctime>
#include <chrono>
#include <iostream>

namespace LegendEngine
{
	namespace ConsoleColor
	{
		static const std::string BLUE = "\x1b[38;2;0;120;220m";
		static const std::string GOLD = "\x1b[38;2;255;213;0m";
		static const std::string GREEN = "\x1b[38;2;0;150;60m";
		static const std::string RED = "\x1b[38;2;255;0;0m";
		static const std::string LIGHT_GRAY = "\x1b[38;2;118;118;118m";
		static const std::string DARK_GRAY = "\x1b[38;2;148;148;148m";
		static const std::string RESET = "\x1b[0m";
	}

	Logger::Logger(const std::string_view name, const bool enabled, const bool debug)
		:
		m_Name(name),
		m_Enabled(enabled),
		m_Debug(debug)
	{}

	void Logger::Log(const Level level, const std::string_view message) const
	{
		if ((level == Level::DEBUG && !m_Debug) || !m_Enabled)
			return;

		const std::string reset = "\x1b[0m";

		std::string severity = "";
		std::string color = "";
		switch (level)
		{
			case Level::INFO:
			{
				severity = "INFO";
				color = ConsoleColor::BLUE;
			}
			break;

			case Level::WARN:
			{
				severity = "WARN";
				color = ConsoleColor::GOLD;
			}
			break;

			case Level::DEBUG:
			{
				severity = "DEBUG";
				color = ConsoleColor::GREEN;
			}
			break;

			case Level::ERROR:
			{
				severity = "ERROR";
				color = ConsoleColor::RED;
			}
			break;
		}

		std::cout
			// Time
			<< ConsoleColor::LIGHT_GRAY
			<< '['
			<< ConsoleColor::DARK_GRAY
			<< GetFormattedTime()
			<< ConsoleColor::LIGHT_GRAY
			<< "] ["
			<< ConsoleColor::DARK_GRAY
			<< m_Name
			<< ConsoleColor::LIGHT_GRAY
			<< "] " << ConsoleColor::RESET
			<< ": " << color
			// Severity
			<< "[" << severity << "] => "
			<< message << ConsoleColor::RESET
		<< std::endl;
	}

	void Logger::SetEnabled(const bool enabled)
	{
		m_Enabled = enabled;
	}

	void Logger::SetDebug(const bool debug)
	{
		m_Debug = debug;
	}

	bool Logger::IsEnabled() const
	{
		return m_Enabled;
	}

	bool Logger::IsDebugEnabled() const
	{
		return m_Debug;
	}

	std::string Logger::GetFormattedTime()
	{
		const auto now = std::chrono::system_clock::now();

		const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		const std::tm* gmtimePtr = gmtime(&currentTime);

		// Time format is HH:MM:SS
		std::string time = "";
		time += std::to_string(gmtimePtr->tm_hour);
		time += ":";
		time += std::to_string(gmtimePtr->tm_min);
		time += ":";
		time += std::to_string(gmtimePtr->tm_sec);

		return time;
	}
}