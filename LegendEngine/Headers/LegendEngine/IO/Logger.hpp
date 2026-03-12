#pragma once

#include <string>
#include <source_location>
#include <format>
#include <memory>
#include <vector>

#ifndef NDEBUG
#define LGENG_INFO(...) LegendEngine::IO::Logger::GetGlobalLogger().Info(__VA_ARGS__)
#define LGENG_WARN(...) LegendEngine::IO::Logger::GetGlobalLogger().Warn(__VA_ARGS__)
#define LGENG_ERROR(...) LegendEngine::IO::Logger::GetGlobalLogger().Error(__VA_ARGS__)
#define LGENG_DEBUG(...) LegendEngine::IO::Logger::GetGlobalLogger().Debug(__VA_ARGS__)
#define LGENG_INFO_TRACE(...) LegendEngine::IO::Logger::GetGlobalLogger().InfoTrace(std::source_location::current(), __VA_ARGS__)
#define LGENG_WARN_TRACE(...) LegendEngine::IO::Logger::GetGlobalLogger().WarnTrace(std::source_location::current(), __VA_ARGS__)
#define LGENG_ERROR_TRACE(...) LegendEngine::IO::Logger::GetGlobalLogger().ErrorTrace(std::source_location::current(), __VA_ARGS__)
#define LGENG_DEBUG_TRACE(...) LegendEngine::IO::Logger::GetGlobalLogger().DebugTrace(std::source_location::current(), __VA_ARGS__)
#else
#define LGENG_INFO(...)
#define LGENG_WARN(...)
#define LGENG_ERROR(...)
#define LGENG_DEBUG(...)
#define LGENG_INFO_TRACE(...)
#define LGENG_WARN_TRACE(...)
#define LGENG_ERROR_TRACE(...)
#define LGENG_DEBUG_TRACE(...)
#endif

namespace LegendEngine::IO
{
	class LoggerSink;
	
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

		explicit Logger(std::string_view name, bool addStdoutSink = true);

		void Log(Level level, std::string_view message, std::optional<std::source_location> location = std::nullopt);

		template <typename... Args>
		void Info(std::format_string<Args...> format, Args&&... args) {
			Log(Level::INFO, std::format(format, std::forward<Args>(args)...));
		}
		
		template <typename... Args>
		void Warn(std::format_string<Args...> format, Args&&... args) {
			Log(Level::WARN, std::format(format, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void Error(std::format_string<Args...> format, Args&&... args) {
			Log(Level::ERROR, std::format(format, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void Debug(std::format_string<Args...> format, Args&&... args) {
			Log(Level::DEBUG, std::format(format, std::forward<Args>(args)...));
		}
		
		template <typename... Args>
		void InfoTrace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			Log(Level::INFO, std::format(format, std::forward<Args>(args)...), location);
		}
		
		template <typename... Args>
		void WarnTrace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			Log(Level::WARN, std::format(format, std::forward<Args>(args)...), location);
		}

		template <typename... Args>
		void ErrorTrace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			Log(Level::ERROR, std::format(format, std::forward<Args>(args)...), location);
		}

		template <typename... Args>
		void DebugTrace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			Log(Level::DEBUG, std::format(format, std::forward<Args>(args)...), location);
		}

		static void CreateGlobalLogger(std::string_view name);
		static void DestroyGlobalLogger();
		static Logger& GetGlobalLogger();
	private:
		static std::string GetFormattedTime();
		static std::string GetFormattedSource(const std::source_location& location);

		std::string m_Name;
		std::vector<std::shared_ptr<LoggerSink>> m_Sinks;

		static std::unique_ptr<Logger> m_Instance;
	};
}