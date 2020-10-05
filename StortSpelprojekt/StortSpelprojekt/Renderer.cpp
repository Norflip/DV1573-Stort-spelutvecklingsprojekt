#include "Renderer.h"
#include "RenderPass.h"

Renderer::Renderer() : device(nullptr), context(nullptr), swapchain(nullptr), obj_cbuffer(nullptr), skeleton_srvbuffer(nullptr),skeleton_srv(nullptr)
{
	srand(unsigned int(time(0)));
}

Renderer::~Renderer()
{
	blendStateOff->Release();
	blendStateOn->Release();

	backbuffer.Release();
	midbuffers[0].Release();
	midbuffers[1].Release();

	for (auto i = passes.begin(); i < passes.end(); i++)
		delete (*i);

	obj_cbuffer->Release();
	skeleton_srvbuffer->Release();
	light_cbuffer->Release();
	material_cbuffer->Release();

	rasterizerStateCullBack->Release();
	rasterizerStateCullNone->Release();

	skeleton_srv->Release();
}

void Renderer::Initialize(Window* window)
{
	this->outputWindow = window;

	DXHelper::CreateSwapchain(*window, &device, &context, &swapchain,&rasterizerStateCullBack, &rasterizerStateCullNone);
	this->backbuffer = DXHelper::CreateBackbuffer(window->GetWidth(), window->GetHeight(), device, swapchain);
	this->midbuffers[0] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device);
	this->midbuffers[1] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device);
	srv_skeleton_data.resize(60);

	dx::XMFLOAT4X4 bone;
	dx::XMStoreFloat4x4(&bone, dx::XMMatrixIdentity());
	for (int boneNr = 0; boneNr < 60; boneNr++)
	{
		srv_skeleton_data[boneNr] = bone;//set id matrix as default for the bones. So if no animation is happening the character is not funky. I need to bind them as well.
	}
	
	DXHelper::CreateConstBuffer(device, &obj_cbuffer, &cb_object_data, sizeof(cb_object_data));
	DXHelper::CreateConstBuffer(device, &light_cbuffer, &cb_scene, sizeof(cb_scene));
	DXHelper::CreateConstBuffer(device, &material_cbuffer, &cb_material_data, sizeof(cb_material_data));
	
	DXHelper::CreateStructuredBuffer(device, &skeleton_srvbuffer, srv_skeleton_data, sizeof(dx::XMFLOAT4X4), srv_skeleton_data.size(), &skeleton_srv);
	DXHelper::BindStructuredBuffer(context, skeleton_srvbuffer, srv_skeleton_data, BONES_SRV_SLOT, ShaderBindFlag::VERTEX, &skeleton_srv);
	DXHelper::CreateBlendState(device, &blendStateOn, &blendStateOff);

	/* Screenquad shader */
	Shader screenQuadShader;
	screenQuadShader.SetPixelShader(L"Shaders/ScreenQuad_ps.hlsl");
	screenQuadShader.SetVertexShader(L"Shaders/ScreenQuad_vs.hlsl");
	screenQuadShader.Compile(device);

	screenQuadMaterial = Material(screenQuadShader);
	screenQuadMaterial.SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, device), 0, ShaderBindFlag::VERTEX);

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
	DrawQueueToTarget(opaqueItemQueue);
}

void Renderer::DrawQueueToTarget(RenderQueue& queue)
{
	for (auto i : queue)
	{
		// bind material from first item in queue
		auto queue = i.second;
		if (!queue.empty())
		{
			queue.front().material->BindToContext(context);

			while (!queue.empty())
			{
				auto item = queue.front();

				switch (item.type)
				{
					case RenderItem::Type::Instanced:
						DrawRenderItemInstanced(item); break;

					case RenderItem::Type::Skeleton:
						DrawRenderItemSkeleton(item); break;

					case RenderItem::Type::Default:
					default:
						DrawRenderItem(item);
						break;
				}

				queue.pop();
			}
		}
	}

	// clear queues
	queue.clear();
}

void Renderer::RenderFrame()
{
	//We need to clear Depth Stencil View as well.//Emil
	size_t bufferIndex = 0;
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRV);

	ClearRenderTarget(midbuffers[bufferIndex]);
	SetRenderTarget(midbuffers[bufferIndex]);
	context->RSSetState(rasterizerStateCullBack);
	context->OMSetBlendState(blendStateOff, BLENDSTATEMASK, 0xffffffff);
	DrawQueueToTarget(opaqueItemQueue);
	context->RSSetState(rasterizerStateCullNone);
	context->OMSetBlendState(blendStateOn, BLENDSTATEMASK, 0xffffffff);
	DrawQueueToTarget(transparentItemQueue);
	context->OMSetBlendState(blendStateOff, BLENDSTATEMASK, 0xffffffff);
	context->RSSetState(rasterizerStateCullBack);
	
	for (auto i = passes.begin(); i < passes.end(); i++)
	{
		if ((*i)->IsEnabled())
		{
			size_t nextBufferIndex = 1 - bufferIndex;
			ClearRenderTarget(midbuffers[nextBufferIndex]);
			SetRenderTarget(midbuffers[nextBufferIndex]);

			GetContext()->PSSetShaderResources(0, 1, &midbuffers[bufferIndex].srv);

			if ((*i)->Pass(this, midbuffers[bufferIndex], midbuffers[nextBufferIndex]))
				bufferIndex = nextBufferIndex;

			// overkill? Gives the correct result if outside the loop but errors in output
			context->PSSetShaderResources(0, 1, nullSRV);
		}
	}

	ClearRenderTarget(backbuffer);
	SetRenderTarget(backbuffer);
	context->PSSetShaderResources(0, 1, &midbuffers[bufferIndex].srv);
	DrawScreenQuad(screenQuadMaterial);
	
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
	item.mesh = &mesh;
	item.material = &material;
	item.type = RenderItem::Type::Default;
	item.world = model;
	item.camera = &camera;
	AddItem(item, material.IsTransparent());
}

void Renderer::DrawInstanced(const Mesh& mesh, const size_t& count, const Material& material, const CameraComponent& camera)
{
	RenderItem item;
	item.mesh = &mesh;
	item.material = &material;
	item.type = RenderItem::Type::Instanced;
	item.instanceCount = count;
	item.camera = &camera;
	AddItem(item, material.IsTransparent());
}

void Renderer::DrawSkeleton(const Mesh& mesh, const Material& material, const dx::XMMATRIX& model, const CameraComponent& camera, std::vector<dx::XMFLOAT4X4>& bones)
{
	
	RenderItem item;
	item.mesh = &mesh;
	item.material = &material;
	item.type = RenderItem::Type::Skeleton;
	item.bones = &bones;
	item.world = model;
	item.camera = &camera;
	AddItem(item, false);
	
}

void Renderer::SetRSToCullNone()
{
	context->RSSetState(rasterizerStateCullNone);
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

void Renderer::AddItem(const RenderItem& item, bool transparent)
{
	RenderQueue& queue = (transparent) ? transparentItemQueue : opaqueItemQueue;

	size_t materialID = item.material->GetID();
	if (queue.find(materialID) == queue.end())
		queue.insert({ materialID, std::queue<RenderItem>() });

	queue[materialID].push(item);
}

void Renderer::DrawRenderItem(const RenderItem& item)
{
	
	dx::XMMATRIX mvp = dx::XMMatrixMultiply(item.world, dx::XMMatrixMultiply(item.camera->GetViewMatrix(), item.camera->GetProjectionMatrix()));
	dx::XMStoreFloat4x4(&cb_object_data.mvp, dx::XMMatrixTranspose(mvp));
	dx::XMStoreFloat4x4(&cb_object_data.world, dx::XMMatrixTranspose(item.world));
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);


	cb_material_data = item.material->GetMaterialData();
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

	/**********************************/

	/* This is a temporary solution to lerp between textures in the shader 
	   This is jsut ot check if the ids (gonna be our different levels later) works
	*/

	static int ids = 0;
	static float color = 0.0f;

	color += (float)0.0005f;
	if (color > 1.0f)
	{
		color -= 1.0f;
		if (ids != 3)
			ids += 1;
		else
			ids = 0;
	}
		
	cb_scene.id = ids;
	cb_scene.factor = color;

	/**********************************/

	cb_scene.nrOfPointLights = 2;
	dx::XMStoreFloat3(&cb_scene.cameraPosition, item.camera->GetOwner()->GetTransform().GetPosition());
	DXHelper::BindConstBuffer(context, light_cbuffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &item.mesh->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->topology);

	context->DrawIndexed(item.mesh->indices.size(), 0, 0);
}

void Renderer::DrawRenderItemInstanced(const RenderItem& item)
{
	
	dx::XMMATRIX vp =dx::XMMatrixMultiply(item.camera->GetViewMatrix(), item.camera->GetProjectionMatrix());
	dx::XMStoreFloat4x4(&cb_object_data.vp, dx::XMMatrixTranspose(vp));
	
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);


	cb_material_data = item.material->GetMaterialData();
	DXHelper::BindConstBuffer(context, material_cbuffer, &cb_material_data, CB_MATERIAL_SLOT, ShaderBindFlag::PIXEL);

	UINT stride[2] = { sizeof(Mesh::Vertex), sizeof(Mesh::InstanceData) };
	UINT offset[2] = { 0 };
	
	context->IASetVertexBuffers(0, 2, item.mesh->vertexAndInstance, stride, offset);
	context->IASetIndexBuffer(item.mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->topology);
	context->DrawIndexedInstanced(item.mesh->indices.size(), item.instanceCount, 0, 0, 0);
}

void Renderer::DrawRenderItemSkeleton(const RenderItem& item)
{
	dx::XMMATRIX mvp = dx::XMMatrixMultiply(item.world, dx::XMMatrixMultiply(item.camera->GetViewMatrix(), item.camera->GetProjectionMatrix()));
	dx::XMStoreFloat4x4(&cb_object_data.mvp, /*dx::XMMatrixTranspose(*/mvp);
	dx::XMStoreFloat4x4(&cb_object_data.world, /*dx::XMMatrixTranspose(*/item.world);
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);

	srv_skeleton_data = *item.bones;
	DXHelper::BindStructuredBuffer(context, skeleton_srvbuffer, srv_skeleton_data, BONES_SRV_SLOT, ShaderBindFlag::VERTEX, &skeleton_srv);

	cb_material_data = item.material->GetMaterialData();
	DXHelper::BindConstBuffer(context, material_cbuffer, &cb_material_data, CB_MATERIAL_SLOT, ShaderBindFlag::PIXEL);
	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &item.mesh->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->topology);

	context->DrawIndexed(item.mesh->indices.size(), 0, 0);
}

void Renderer::DrawScreenQuad(const Material& material)
{
	
	
	material.BindToContext(context);
	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &screenQuadMesh.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(screenQuadMesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(screenQuadMesh.indices.size(), 0, 0);
}
