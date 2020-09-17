#include "Engine.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	Engine engine (hInstance);
	engine.Run();
	return 0;
}
