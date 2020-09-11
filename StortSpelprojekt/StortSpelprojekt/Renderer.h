#pragma once
#include "DXHandler.h"
#include "Mesh.h"
#include "ConstantBuffer.h"
namespace dx = DirectX;

class Renderer
{
	const FLOAT DEFAULT_BG_COLOR[4] = { 0.3f, 0.1f, 0.2f, 1.0f };

public:
	Renderer();
	virtual ~Renderer();

	void Initialize(DXHandler dxHandler);
	void BeginFrame();
	void EndFrame();
	
	void Draw (const Mesh& mesh, dx::XMMATRIX model, dx::XMMATRIX view, dx::XMMATRIX projection);

private:
	DXHandler dxHandler;
	ObjectConstantBuffer objectBuffer;

};