#pragma once
#include <chrono>
#include <assert.h>

#include "DXHandler.h"
#include "Window.h"
#include "Log.h"
#include "ShittyOBJLoader.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "Object.h"

namespace dx = DirectX;

class Engine
{
public:
	Engine(Window& window);
	virtual ~Engine();
	void Run();

private:
	void TMP_Update(const float& deltaTime);

private:
	Window& window;
	DXHandler dxHandler;
	Renderer renderer;
	Camera camera;
	Object tmp_obj;
};