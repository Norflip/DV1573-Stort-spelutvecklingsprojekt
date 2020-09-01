#pragma once
#include <string>
#include <fcntl.h>
#include <io.h>
#include <iostream>

#define OPEN_LOG

namespace Log
{
	bool m_logOpen = false;

	inline void Open()
	{
#if defined(OPEN_LOG) && _DEBUG
		AllocConsole();
		HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		int hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		FILE* fp = _fdopen(hConsole, "w");
		freopen_s(&fp, "CONOUT$", "w", stdout);
		m_logOpen = true;
#endif
	}

	inline void Add(const std::string& msg)
	{
#if defined(OPEN_LOG) && _DEBUG
		if (m_logOpen)
			std::cout << msg << std::endl;
#endif
	}
}