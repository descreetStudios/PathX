#pragma once

#include <PathXAPI/Core.h>

void run()
{
	using namespace pathx;
	using namespace logger;

	set_level(LoggerLevel::Info);

	log("Hello world!");
	log("Error Message!", LoggerLevel::Error);
	log("Trace Message!", LoggerLevel::Trace);
	log("Only DebugOutput Message!", LoggerMode::DebugOutput);
}