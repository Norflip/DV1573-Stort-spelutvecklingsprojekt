#include "Renderer.h"

Renderer::Renderer() : device(nullptr), context(nullptr), backbuffer(nullptr), swapchain(nullptr), obj_cbuffer(nullptr)
{
}

Renderer::~Renderer()
{
}

void Renderer::Initialize(Window& window)
{
	DXHelper::CreateSwapchain(window, &device, &context, &swapchain, &backbuffer);
	
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Width = static_cast<FLOAT>(window.GetWidth());
	viewport.Height = static_cast<FLOAT>(window.GetHeight());
	context->RSSetViewports(1, &viewport);

	DXHelper::CreateCBuffer(device, &obj_cbuffer, &cb_object_data, sizeof(cb_object_data));
}

void Renderer::BeginFrame()
{
	// CLEAR THAT SHIIIET
	context->ClearRenderTargetView(backbuffer, DEFAULT_BG_COLOR);
}

void Renderer::EndFrame()
{
	swapchain->Present(0, 0);
}

void Renderer::Draw(const Mesh& mesh, dx::XMMATRIX model, dx::XMMATRIX view, dx::XMMATRIX projection)
{
	// add to queue? 

	dx::XMStoreFloat4x4(&cb_object_data.mvp, dx::XMMatrixTranspose(model * view * projection));
	dx::XMStoreFloat4x4(&cb_object_data.world, dx::XMMatrixTranspose(model));
	DXHelper::BindCBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

//	context->DrawIndexed(mesh.indices.size(), 0, 0);

	context->DrawIndexedInstanced(mesh.indices.size(), 10, 0, 0, 0);

}

void Renderer::DrawInstanced(const Mesh& mesh, size_t count, dx::XMMATRIX* models, dx::XMMATRIX view, dx::XMMATRIX projection)
{
	// another cbuffer for instanced? 


	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(mesh.topology);

	context->DrawIndexedInstanced(mesh.indices.size(), count, 0, 0, 0);
}
