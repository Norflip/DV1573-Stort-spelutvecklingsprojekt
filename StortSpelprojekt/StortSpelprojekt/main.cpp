#include "stdafx.h"
#include "Engine.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	// Use this to search for memory leak locations
	//_crtBreakAlloc = 650485;

	Engine engine(hInstance);

	engine.Run();

	return 0;
}
