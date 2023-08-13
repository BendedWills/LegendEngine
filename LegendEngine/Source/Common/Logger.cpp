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

	Logger::Logger(std::string_view name)
		:
		m_Name(name)
	{}

	void Logger::Log(Level level, std::string_view message)
	{
		// If debug isn't enabled, disable debug logs.
		if ((level == Level::DEBUG && !debug) || !logging)
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

	std::string Logger::GetFormattedTime()
	{
		auto now = std::chrono::system_clock::now();

		std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		std::tm* orgtimePtr = gmtime(&currentTime);

		// Time format is HH:MM:SS
		std::string time = "";
		time += std::to_string(orgtimePtr->tm_hour);
		time += ":";
		time += std::to_string(orgtimePtr->tm_min);
		time += ":";
		time += std::to_string(orgtimePtr->tm_sec);

		return time;
	}
}