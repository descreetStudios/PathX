#pragma once

#include "EngineCommon.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !defined(WIN32_LEAN_AND_MEAN)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

namespace pathx
{
#pragma region IO

	inline Path get_exe_path()
	{
#if defined(_WIN32)

		WString buffer(MAX_PATH, L'\0');
		DWORD size{ GetModuleFileNameW(nullptr, buffer.data(),
			static_cast<DWORD>(buffer.size())) };
		assert(size != 0);

		buffer.resize(size);
		return Path(buffer);

#elif defined(__APPLE__)

		u32 size{ 0u };
		_NSGetExecutablePath(nullptr, &size); // get required size

		String buffer(size, '\0');
		u8 failed{ _NSGetExecutablePath(buffer.data(), &size) };
		assert(!failed);

		return fs::canonical(buffer);

#elif defined(__linux__)

		String buffer(1024, '\0');
		ssize_t len{ readlink("/proc/self/exe", buffer.data(), buffer.size()) };
		assert(len != -1);

		buffer.resize(len);
		return fs::canonical(buffer);

#else
#error Unsupported platform
#endif
	}

	inline void set_working_directory_to_exe_path()
	{
		Path exe{ get_exe_path() };
		Path dir{ exe.parent_path() };

		fs::current_path(dir);
	}

#pragma endregion
} // pathx