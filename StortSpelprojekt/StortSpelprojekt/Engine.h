#pragma once


#include <chrono>
#include <d3d11_1.h>
#include <dxgi.h> 
#include <assert.h>
#include <d3dcompiler.h>

#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment( lib, "dxgi" )   
#pragma comment(lib, "d3dcompiler.lib")

#include "Window.h"
#include "Log.h"
#include "ShittyOBJLoader.h"
#include "Material.h"
#include "Transform.h"
#include "Camera.h"
#include "ZWEBLoader.h"

namespace dx = DirectX;

__declspec(align(16))
struct WorldData
{
	DirectX::XMFLOAT4X4 mvp;
	DirectX::XMFLOAT4X4 world; //I changed this to fix padding
};




class Engine
{
public:
	Engine(Window& window);
	virtual ~Engine();
	void Run();

private:
	void TMP_SetupDX11(HWND hwnd, size_t width, size_t height);
	void TMP_Update(const float& deltaTime,float elapsedTime);
	void TMP_DrawMesh(Mesh& mesh, const Transform& transform, const Camera& camera, float elapsedTime);

private:
	Window& window;
	ID3D11RenderTargetView* backbufferRTV;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapchain;

	Mesh mesh;
	Transform transform;

	Camera camera;
	Material material;

	ID3D11Buffer* worldBuffer_ptr;
	WorldData cb_world;

	ID3D11Buffer* skeletonBuffer_ptr;
	SkeletonData cb_skeleton;
};