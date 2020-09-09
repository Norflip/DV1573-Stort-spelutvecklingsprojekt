#include "Renderer.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Initialize(DXHandler dxHandler)
{
	this->dxHandler = dxHandler;
	this->objectBuffer.Initialize(CB_OBJECT_SLOT, ShaderTypeFlag::VERTEX, dxHandler.GetDevice());
}

void Renderer::BeginFrame()
{
	// CLEAR THAT SHIIIET
	dxHandler.GetContext()->ClearRenderTargetView(dxHandler.GetBackbuffer(), DEFAULT_BG_COLOR);
}

void Renderer::EndFrame()
{
	dxHandler.GetSwapchain()->Present(0, 0);
}

void Renderer::Draw(const Mesh& mesh, dx::XMMATRIX model, dx::XMMATRIX view, dx::XMMATRIX projection)
{
	auto cb_objectData = objectBuffer.GetData();

	dx::XMStoreFloat4x4(&cb_objectData->mvp, dx::XMMatrixTranspose(dx::XMMatrixMultiply(dx::XMMatrixMultiply(model, view), projection)));
	dx::XMStoreFloat4x4(&cb_objectData->world, dx::XMMatrixTranspose(model));
	objectBuffer.Bind(dxHandler.GetContext());

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	dxHandler.GetContext()->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
	dxHandler.GetContext()->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dxHandler.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//context->DrawIndexed(mesh.indices.size(), 0, 0);

	dxHandler.GetContext()->DrawIndexedInstanced(mesh.indices.size(), 10, 0, 0, 0);
}
