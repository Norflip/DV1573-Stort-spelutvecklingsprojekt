#include "stdafx.h"
#include "Engine.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Use this to search for memory leak locations
	//_crtBreakAlloc = 2268518;

	Engine engine(hInstance);

	engine.Run();

	return 0;
}
