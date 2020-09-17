#include "Renderer.h"

Renderer::Renderer() : device(nullptr), context(nullptr), backbuffer(nullptr), swapchain(nullptr), obj_cbuffer(nullptr), skeleton_cbuffer(nullptr)
{
}

Renderer::~Renderer()
{
	skeleton_cbuffer->Release();
}

void Renderer::Initialize(Window* window)
{
	this->outputWindow = window;
	DXHelper::CreateSwapchain(*window, &device, &context, &swapchain);
	DXHelper::CreateBackbuffer(window->GetWidth(), window->GetHeight(), device, swapchain, &backbuffer, &depthStencilView);
	
	context->OMSetRenderTargets(1, &backbuffer, depthStencilView);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Width = static_cast<FLOAT>(window->GetWidth());
	viewport.Height = static_cast<FLOAT>(window->GetHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	DXHelper::CreateConstBuffer(device, &obj_cbuffer, &cb_object_data, sizeof(cb_object_data));

	
	for (int bone = 0; bone < 60; bone++) //set id matrix as default for the bones. So if no animation is happening the character is not funky.
	{
		dx::XMStoreFloat4x4(&cb_skeleton_data.bones[bone], dx::XMMatrixIdentity());
	}


	DXHelper::CreateConstBuffer(device, &skeleton_cbuffer, &cb_skeleton_data, sizeof(cb_Skeleton));
}

void Renderer::BeginFrame()
{
	context->ClearRenderTargetView(backbuffer, DEFAULT_BG_COLOR);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::EndFrame()
{
	swapchain->Present(0, 0);
}

void Renderer::Draw(const Mesh& mesh, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection)
{
	// add to queue? 

	dx::XMMATRIX mvp = dx::XMMatrixMultiply(world, dx::XMMatrixMultiply(view, projection));
	dx::XMStoreFloat4x4(&cb_object_data.mvp, dx::XMMatrixTranspose(mvp));
	dx::XMStoreFloat4x4(&cb_object_data.world, dx::XMMatrixTranspose(world));
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(mesh.topology);

	context->DrawIndexed(mesh.indices.size(), 0, 0);
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

void Renderer::DrawSkeleton(const Mesh& mesh, dx::XMMATRIX model, dx::XMMATRIX view, dx::XMMATRIX projection, const std::vector<dx::XMFLOAT4X4>& bones)
{


	dx::XMMATRIX mvp = dx::XMMatrixMultiply(model, dx::XMMatrixMultiply(view, projection));
	dx::XMStoreFloat4x4(&cb_object_data.mvp, dx::XMMatrixTranspose(mvp));
	dx::XMStoreFloat4x4(&cb_object_data.world, dx::XMMatrixTranspose(model));
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);

	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_SKELETON_SLOT, ShaderBindFlag::SKELETON);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(mesh.topology);

	context->DrawIndexed(mesh.indices.size(), 0, 0);




}
