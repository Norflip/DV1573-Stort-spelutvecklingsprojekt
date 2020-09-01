#include "Window.h"
#include "Log.h"


#include "BusNode.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	Window window (hInstance);
	window.Open(800, 800);

	Log::Open();
	Log::Add("Test output");

	MessageBus messageBus;
	ComponentA compA(&messageBus);
	ComponentB compB(&messageBus);

	// This is supposed to act like a game loop.
	for (int ctr = 0; ctr < 10; ctr++) 
	{
		compA.Update();
		compB.Update();
	
		messageBus.Notify();
	}
	
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
