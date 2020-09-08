#pragma once
#include <chrono>
#include <assert.h>

#include "DXHandler.h"
#include "Window.h"
#include "Log.h"
#include "ShittyOBJLoader.h"
#include "Material.h"
#include "Transform.h"
#include "Camera.h"
#include "ConstantBuffer.h"

namespace dx = DirectX;

__declspec(align(16))
struct WorldData
{
	DirectX::XMMATRIX mvp;
	DirectX::XMMATRIX world;
};

class Engine
{
public:
	Engine(Window& window);
	virtual ~Engine();
	void Run();

private:
	void TMP_Update(const float& deltaTime);
	void TMP_DrawMesh(const Mesh& mesh, const Transform& transform, const Camera& camera);

private:
	Window& window;
	DXHandler dxHandler;

	Mesh mesh;
	Transform transform;

	Camera camera;
	Material material;

	ConstantBuffer<WorldData> objectBuffer;
};