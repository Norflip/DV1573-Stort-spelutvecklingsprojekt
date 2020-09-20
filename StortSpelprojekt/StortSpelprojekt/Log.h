#pragma once
#include <string>
#include <fcntl.h>
#include <io.h>
#include <iostream>

#include <cstring>
#include <fstream>

#define OPEN_LOG  1

namespace Log
{
	static bool m_logOpen = false;

	enum class LogLevel
	{
		Info,
		Warning,
		Error,
		Debug
	};

	inline void Open()
	{
		#if (OPEN_LOG)
				AllocConsole();
				HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
				int hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
				FILE* fp = _fdopen(hConsole, "w");
				freopen_s(&fp, "CONOUT$", "w", stdout);
				m_logOpen = true;
		#endif
	}

	inline void Add(LogLevel level, const std::string& msg)
	{
#if defined(OPEN_LOG)

		std::string prefix = "";
		switch (level)
		{
			case Log::LogLevel::Warning: prefix = "Warning";
				break;
			case Log::LogLevel::Error: prefix = "Error";
				break;
			case Log::LogLevel::Debug: prefix = "Debug";
				break;
			default:
			case Log::LogLevel::Info: prefix = "INFO";
				break;
		}

		std::cout << "[" + prefix + "] " << msg << "\n";
#endif
	}

	inline void Add(const std::string& msg)
	{
#if defined(OPEN_LOG)
		if (!m_logOpen)
			Open();

		Add(LogLevel::Info, msg);
#endif
	}

	inline void AddRaw(const std::string& msg)
	{
#if defined(OPEN_LOG)
		if (!m_logOpen)
			Open();

		std::cout << msg << "\n";
#endif
	}
}