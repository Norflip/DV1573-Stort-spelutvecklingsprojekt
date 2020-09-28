#include "Renderer.h"
#include "RenderPass.h"

Renderer::Renderer() : device(nullptr), context(nullptr), swapchain(nullptr), obj_cbuffer(nullptr), skeleton_cbuffer(nullptr)
{
}

Renderer::~Renderer()
{
	backbuffer.Release();
	midbuffers[0].Release();
	midbuffers[1].Release();

	for (auto i = passes.begin(); i < passes.end(); i++)
		delete (*i);

	obj_cbuffer->Release();
	skeleton_cbuffer->Release();
	light_cbuffer->Release();
	material_cbuffer->Release();
}

void Renderer::Initialize(Window* window)
{
	this->outputWindow = window;

	DXHelper::CreateSwapchain(*window, &device, &context, &swapchain);
	this->backbuffer = DXHelper::CreateBackbuffer(window->GetWidth(), window->GetHeight(), device, swapchain);
	this->midbuffers[0] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device);
	this->midbuffers[1] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device);

	for (int bone = 0; bone < 60; bone++) //set id matrix as default for the bones. So if no animation is happening the character is not funky.
	{
		dx::XMStoreFloat4x4(&cb_skeleton_data.bones[bone], dx::XMMatrixIdentity());
	}

	DXHelper::CreateConstBuffer(device, &obj_cbuffer, &cb_object_data, sizeof(cb_object_data));
	DXHelper::CreateConstBuffer(device, &light_cbuffer, &cb_scene, sizeof(cb_scene));
	DXHelper::CreateConstBuffer(device, &material_cbuffer, &cb_material_data, sizeof(cb_material_data));
	DXHelper::CreateConstBuffer(device, &skeleton_cbuffer, &cb_skeleton_data, sizeof(cb_skeleton_data));

	/* Screenquad shader */
	screenQuadShader.SetPixelShader(L"Shaders/ScreenQuad_ps.hlsl");
	screenQuadShader.SetVertexShader(L"Shaders/ScreenQuad_vs.hlsl");
	screenQuadShader.Compile(device);

	/* Screenquad mesh */
	screenQuadMesh = Mesh::CreateScreenQuad(device);

	//EXEMPEL
	///AddRenderPass(new PSRenderPass(1, L"Shaders/TestPass.hlsl"));
	//AddRenderPass(new PSRenderPass(0, L"Shaders/TestPass2.hlsl"));
}

void Renderer::BeginManualRenderPass(RenderTexture& target)
{
	SetRenderTarget(target);
}

void Renderer::EndManualRenderPass()
{
	DrawItemsToTarget();
}

void Renderer::DrawItemsToTarget()
{
	for (auto i : itemQueue)
	{
		// bind material from first item in queue
		auto queue = i.second;
		if (!queue.empty())
		{
			queue.front().material.BindToContext(context);

			while (!queue.empty())
			{
				auto item = queue.front();

				switch (item.type)
				{
					case RenderItem::Type::Instanced:
						m_DrawInstanced(item); break;

					case RenderItem::Type::Skeleton:
						m_DrawSkeleton(item); break;

					case RenderItem::Type::Default:
					default:
						m_Draw(item);
						break;
				}

				queue.pop();
			}
		}
	}

	// clear queues
	itemQueue.clear();
}

void Renderer::RenderFrame()
{
	size_t bufferIndex = 0;
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRV);

	ClearRenderTarget(midbuffers[bufferIndex]);
	SetRenderTarget(midbuffers[bufferIndex]);
	DrawItemsToTarget();

	for (auto i = passes.begin(); i < passes.end(); i++)
	{
		if ((*i)->IsEnabled())
		{
			size_t nextBufferIndex = 1 - bufferIndex;
			ClearRenderTarget(midbuffers[nextBufferIndex]);
			SetRenderTarget(midbuffers[nextBufferIndex]);

			GetContext()->PSSetShaderResources(0, 1, &midbuffers[bufferIndex].srv);

			(*i)->Pass(this, midbuffers[bufferIndex], midbuffers[nextBufferIndex]);
			bufferIndex = nextBufferIndex;

			// overkill? Gives the correct result if outside the loop but errors in output
			context->PSSetShaderResources(0, 1, nullSRV);
		}
	}

	ClearRenderTarget(backbuffer);
	SetRenderTarget(backbuffer);
	context->PSSetShaderResources(0, 1, &midbuffers[bufferIndex].srv);
	DrawScreenQuad(screenQuadShader);

	HRESULT hr = swapchain->Present(0, 0); //1 here?
	assert(SUCCEEDED(hr));
}

void Renderer::AddRenderPass(RenderPass* pass)
{
	pass->m_Initialize(device);
	passes.push_back(pass);

	if (passes.size() > 1)
		std::sort(passes.begin(), passes.end(), [](const RenderPass* a, const RenderPass* b) -> bool { return a->GetPriority() < b->GetPriority(); });
}

void Renderer::Draw(const Mesh& mesh, const Material& material, const dx::XMMATRIX& model, const CameraComponent& camera)
{
	RenderItem item;
	item.mesh = mesh;
	item.material = material;
	item.type = RenderItem::Type::Default;
	item.world = model;
	item.camera = &camera;
	AddItem(item);
}

void Renderer::DrawInstanced(const Mesh& mesh, size_t count, const Material& material, const dx::XMMATRIX& model, const CameraComponent& camera)
{
	RenderItem item;
	item.mesh = mesh;
	item.material = material;
	item.type = RenderItem::Type::Instanced;
	item.instanceCount = count;
	item.world = model;
	item.camera = &camera;
	AddItem(item);
}

void Renderer::DrawSkeleton(const Mesh& mesh, const Material& material, const dx::XMMATRIX& model, const CameraComponent& camera, cb_Skeleton& bones)
{
	RenderItem item;
	item.mesh = mesh;
	item.material = material;
	item.type = RenderItem::Type::Skeleton;
	item.bones = bones;
	item.world = model;
	item.camera = &camera;
	AddItem(item);
}

void Renderer::ClearRenderTarget(const RenderTexture& target)
{
	context->ClearRenderTargetView(target.rtv, DEFAULT_BG_COLOR);
	context->ClearDepthStencilView(target.dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::SetRenderTarget(const RenderTexture& target)
{
	context->OMSetRenderTargets(1, &target.rtv, target.dsv);
	context->RSSetViewports(1, &target.viewport);
}

void Renderer::AddItem(const RenderItem& item)
{
	size_t materialID = item.material.GetID();
	if (itemQueue.find(materialID) == itemQueue.end())
		itemQueue.insert({ materialID, std::queue<RenderItem>() });

	itemQueue[materialID].push(item);
}

void Renderer::m_Draw(const RenderItem& item)
{
	// add to queue? 
	dx::XMMATRIX mvp = dx::XMMatrixMultiply(item.world, dx::XMMatrixMultiply(item.camera->GetViewMatrix(), item.camera->GetProjectionMatrix()));
	dx::XMStoreFloat4x4(&cb_object_data.mvp, dx::XMMatrixTranspose(mvp));
	dx::XMStoreFloat4x4(&cb_object_data.world, dx::XMMatrixTranspose(item.world));
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);


	cb_material_data = item.material.GetMaterialData();
	DXHelper::BindConstBuffer(context, material_cbuffer, &cb_material_data, CB_MATERIAL_SLOT, ShaderBindFlag::PIXEL);


	cb_scene.sunDirection = dx::XMFLOAT3(0.0f, 100.0f, -45.0f);
	cb_scene.sunIntensity = 0.4f;

	cb_scene.pointLights[0].lightColor = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	cb_scene.pointLights[0].lightPosition = dx::XMFLOAT3(15.0f, -5.0f, -5.0f);
	cb_scene.pointLights[0].attenuation = dx::XMFLOAT3(1.0f, 0.02f, 0.0f);
	cb_scene.pointLights[0].range = 25.0f;

	cb_scene.pointLights[1].lightColor = dx::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	cb_scene.pointLights[1].lightPosition = dx::XMFLOAT3(-10.0f, 10.0f, -5.0f);
	cb_scene.pointLights[1].attenuation = dx::XMFLOAT3(1.0f, 0.02f, 0.0f);
	cb_scene.pointLights[1].range = 25.0f;

	cb_scene.nrOfPointLights = 2;
	dx::XMStoreFloat3(&cb_scene.cameraPosition, item.camera->GetOwner()->GetTransform().GetPosition());
	DXHelper::BindConstBuffer(context, light_cbuffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &item.mesh.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh.topology);

	context->DrawIndexed(item.mesh.indices.size(), 0, 0);
}

void Renderer::m_DrawInstanced(const RenderItem& item)
{
	// another cbuffer for instanced? 
	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &item.mesh.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh.topology);
	context->DrawIndexedInstanced(item.mesh.indices.size(), item.instanceCount, 0, 0, 0);
}

void Renderer::m_DrawSkeleton(const RenderItem& item)
{
	dx::XMMATRIX mvp = dx::XMMatrixMultiply(item.world, dx::XMMatrixMultiply(item.camera->GetViewMatrix(), item.camera->GetProjectionMatrix()));
	dx::XMStoreFloat4x4(&cb_object_data.mvp, dx::XMMatrixTranspose(mvp));
	dx::XMStoreFloat4x4(&cb_object_data.world, dx::XMMatrixTranspose(item.world));
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);

	cb_skeleton_data = item.bones;
	DXHelper::BindConstBuffer(context, skeleton_cbuffer, &cb_skeleton_data, CB_SKELETON_SLOT, ShaderBindFlag::VERTEX);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &item.mesh.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh.topology);

	context->DrawIndexed(item.mesh.indices.size(), 0, 0);
}

void Renderer::DrawScreenQuad(const Shader& shader)
{
	shader.BindToContext(context);
	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &screenQuadMesh.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(screenQuadMesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(screenQuadMesh.indices.size(), 0, 0);
}
