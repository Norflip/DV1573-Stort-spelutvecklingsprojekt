#define NOMINMAX
#include "Engine.h"


#include <reactphysics3d.h> 
namespace rp = reactphysics3d;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Engine engine(hInstance);

#if USE_REACT_INSTEAD_OF_BULLET
	rp::PhysicsCommon physicsCommon;

	// Create a physics world 
	rp::PhysicsWorld* world = physicsCommon.createPhysicsWorld();

	
	// Use this to search for memory leak locations
	//_crtBreakAlloc = 2348;

		

	bool isnu = world == nullptr;
	for (size_t i = 0; i < 40; i++)
	{
		std::cout << "null: " << isnu << std::endl;
	}
#endif

	engine.Run();	

	return 0;
}
