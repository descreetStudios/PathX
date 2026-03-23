#pragma once

#include "EngineCommon.h"

namespace pathx
{
	enum class LoggerLevel
	{
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	enum class LoggerMode
	{
		Console,
		DebugOutput,
		Both
	};

	namespace logger
	{
		void log(const String& message,
			LoggerMode mode = LoggerMode::Both,
			LoggerLevel level = LoggerLevel::Info);
		void log(const String& message,
			LoggerLevel level);

		void set_level(LoggerLevel level);
	} // pathx::logger
} // pathx