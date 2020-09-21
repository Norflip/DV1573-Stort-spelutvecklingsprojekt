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


	DXHelper::CreateConstBuffer(device, &skeleton_cbuffer, &cb_skeleton_data, sizeof(cb_skeleton_data));
	DXHelper::CreateConstBuffer(device, &light_cbuffer, &cb_scene, sizeof(cb_scene));
	DXHelper::CreateConstBuffer(device, &material_cbuffer, &cb_material_data, sizeof(cb_material_data));	
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

void Renderer::RenderToTexture(Texture* texture, ID3D11Device* device, int textureWidth, int textureHeight)
{	
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&textureDesc, NULL, &renderTexture);
	assert(SUCCEEDED(hr));
	

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(renderTexture, &renderTargetViewDesc, &rtvTest);
	assert(SUCCEEDED(hr));


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(renderTexture, &shaderResourceViewDesc, &srvTest);
	assert(SUCCEEDED(hr));


	texture->SetTexture(srvTest);
	texture->SetRenderTarget(rtvTest);
}

void Renderer::SetRenderTarget(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv)
{
	context->OMSetRenderTargets(1, &rtv, depthStencilView);
}

void Renderer::SetBackbufferRenderTarget()
{
	context->OMSetRenderTargets(1, &backbuffer, depthStencilView);
}

void Renderer::ClearRenderTarget(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, dx::XMFLOAT4 rgba)
{
	float color[4];
	color[0] = rgba.x;
	color[1] = rgba.y;
	color[2] = rgba.z;
	color[3] = rgba.w;

	context->ClearRenderTargetView(rtv, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::Unbind()
{	
	/*	
		Store total srvs for unbinding later
		At this moment, 2 slots (diffuse, normal)
	*/
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	for(int i = 0; i < 2; i++)
		context->PSSetShaderResources(i, 1, nullSRV);	
}

void Renderer::Draw(const Mesh& mesh, const cb_Material& material, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection)
{
	// add to queue? 

	dx::XMMATRIX mvp = dx::XMMatrixMultiply(world, dx::XMMatrixMultiply(view, projection));
	dx::XMStoreFloat4x4(&cb_object_data.mvp, dx::XMMatrixTranspose(mvp));
	dx::XMStoreFloat4x4(&cb_object_data.world, dx::XMMatrixTranspose(world));
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);
	
	cb_material_data.ambient = material.ambient;
	cb_material_data.diffuse = material.diffuse;
	cb_material_data.specular = material.specular;
	cb_material_data.hasAlbedo = false;
	cb_material_data.hasNormalMap = false;

	DXHelper::BindConstBuffer(context, material_cbuffer, &cb_material_data, CB_MATERIAL_SLOT, ShaderBindFlag::PIXEL);

	cb_scene.pointLights[0].lightColor = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	cb_scene.pointLights[0].lightPosition = dx::XMFLOAT3(5.0f, 5.0f, -10.0f);
	cb_scene.pointLights[0].attenuation = dx::XMFLOAT3(1.0f, 0.02f, 0.0f);
	cb_scene.pointLights[0].range = 25.0f;
	DXHelper::BindConstBuffer(context, light_cbuffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);

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
