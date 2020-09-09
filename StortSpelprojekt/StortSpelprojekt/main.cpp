#include "Window.h"
#include "Log.h"
#include "Engine.h"

#include "BusNode.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	Log::Open();

	Window window (hInstance);
	window.Open(800, 800);

	Engine engine(window);
	engine.Run();


	// kolla över
	//MessageBus messageBus;
	//ComponentA compA(&messageBus);
	//ComponentB compB(&messageBus);

	//// This is supposed to act like a game loop.
	//for (int ctr = 0; ctr < 10; ctr++) 
	//{
	//	compA.Update();
	//	compB.Update();
	//
	//	messageBus.Notify();
	//}	

	return 0;
}
