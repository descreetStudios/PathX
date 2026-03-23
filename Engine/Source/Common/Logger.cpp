#include "Logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>

namespace pathx::logger
{
	namespace
	{
		LoggerLevel g_level{ LoggerLevel::Info };

		SharedPtr<spdlog::logger> g_logger{ nullptr };

		spdlog::sink_ptr g_consoleSink;
		spdlog::sink_ptr g_debugSink;

		class CapitalisedLevelFormatter : public spdlog::custom_flag_formatter
		{
		public:
			void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override
			{
				String level{ spdlog::level::to_string_view(msg.level).data() };
				level[0] = static_cast<char>(std::toupper(level[0]));
				dest.append(level.data(), level.data() + level.size());
			}

			UniquePtr<custom_flag_formatter> clone() const override
			{
				return spdlog::details::make_unique<CapitalisedLevelFormatter>();
			}
		};

		[[nodiscard]]
		spdlog::level::level_enum to_spd_level(LoggerLevel level)
		{
			return static_cast<spdlog::level::level_enum>(level);
		}

		void ensure_init()
		{
			if (g_logger) return;

			g_consoleSink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
			g_debugSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

			g_logger = std::make_shared<spdlog::logger>(
				"Engine",
				spdlog::sinks_init_list{ g_consoleSink, g_debugSink }
			);

			g_logger->set_level(to_spd_level(g_level));

			auto formatter = std::make_unique<spdlog::pattern_formatter>();
			formatter->add_flag<CapitalisedLevelFormatter>('*');
			formatter->set_pattern("[%T] [%^%*%$] %v");
			g_logger->set_formatter(std::move(formatter));
		}
	} // anonymous

	void log(const String& message, LoggerMode mode, LoggerLevel level)
	{
		ensure_init();

		auto lvl = to_spd_level(level);
		if (lvl < g_logger->level()) return;

		auto msg = spdlog::details::log_msg({}, "Engine", lvl, message);

		if (mode == LoggerMode::Console || mode == LoggerMode::Both)
			g_consoleSink->log(msg);
		if (mode == LoggerMode::DebugOutput || mode == LoggerMode::Both)
			g_debugSink->log(msg);
	}

	void log(const String& message, LoggerLevel level)
	{
		log(message, LoggerMode::Both, level);
	}

	void set_level(LoggerLevel level)
	{
		// NOTE: We set g_level here for compatibility with first initialization
		g_level = level;

		if (g_logger)
			g_logger->set_level(to_spd_level(level));
	}
} // pathx::logger