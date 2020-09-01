#include "Window.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	Window window (hInstance);
	window.Open(800, 800);
	std::cout << "awdjaiwdjaw" << std::endl;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (TRUE)
	{
		// Redirects the messages to the static WindowProc in Window.cpp
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
	}

	return 0;
}
