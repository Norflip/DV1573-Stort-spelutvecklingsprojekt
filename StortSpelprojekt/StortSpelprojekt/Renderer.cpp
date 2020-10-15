#include "Renderer.h"
#include "RenderPass.h"
#include "FogRenderPass.h"
#include "DShape.h"

Renderer::Renderer() : device(nullptr), context(nullptr), swapchain(nullptr), obj_cbuffer(nullptr), skeleton_srvbuffer(nullptr),skeleton_srv(nullptr)
{
	srand(unsigned int(time(0)));
}

Renderer::~Renderer()
{
	blendStateOff->Release();
	blendStateOn->Release();

	backbuffer.Release();
	renderPassSwapBuffers[0].Release();
	renderPassSwapBuffers[1].Release();
	midbuffer.Release();

	for (auto i = passes.begin(); i < passes.end(); i++)
		delete (*i);

	obj_cbuffer->Release();

	//skeleton_cbuffer->Release();

	skeleton_srvbuffer->Release();

	material_cbuffer->Release();

	rasterizerStateCullBack->Release();
	rasterizerStateCullNone->Release();

	skeleton_srv->Release();
	dss->Release();
}

void Renderer::Initialize(Window* window)
{

	if (sizeof(cb_grass) % 16 != 0)
	{
		OutputDebugStringA("WRONG WITH GRASS");
	}
	if (sizeof(cb_Lights) % 16 != 0)
	{
		OutputDebugStringA("WRONG WITH LIGHT");
	}
	if (sizeof(cb_Material) % 16 != 0)
	{
		OutputDebugStringA("WRONG WITH MATERIAL");
	}
	if (sizeof(cb_Object) % 16 != 0)
	{
		OutputDebugStringA("WRONG WITH Object");
	}
	if (sizeof(cb_Scene) % 16 != 0)
	{
		OutputDebugStringA("WRONG WITH SCENE");
	}
	if (sizeof(s_PointLight) % 16 != 0)
	{
		OutputDebugStringA("WRONG WITH GRASS");
	}
	if (sizeof(cb_grass) % 16 != 0)
	{
		OutputDebugStringA("WRONG WITH PLIGHT");
	}
	


	this->outputWindow = window;

	DXHelper::CreateSwapchain(*window, &device, &context, &swapchain);
	this->backbuffer = DXHelper::CreateBackbuffer(window->GetWidth(), window->GetHeight(), device, swapchain);

	this->renderPassSwapBuffers[0] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device, context, &dss);
	this->renderPassSwapBuffers[1] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device, context, &dss);
	srv_skeleton_data.resize(60);

	midbuffer = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device, context, &dss);

	dx::XMFLOAT4X4 bone;
	dx::XMStoreFloat4x4(&bone, dx::XMMatrixIdentity());
	
	DXHelper::CreateRSState(device, &rasterizerStateCullBack, &rasterizerStateCullNone);


	for (int boneNr = 0; boneNr < 60; boneNr++) //set id matrix as default for the bones. So if no animation is happening the character is not funky.
	{
		srv_skeleton_data[boneNr] = bone;//set id matrix as default for the bones. So if no animation is happening the character is not funky. I need to bind them as well.
	}
	
	DXHelper::CreateConstBuffer(device, &obj_cbuffer, &cb_object_data, sizeof(cb_object_data));
	LightManager::Instance().Initialize(device);
	DXHelper::CreateConstBuffer(device, &material_cbuffer, &cb_material_data, sizeof(cb_material_data));
	
	DXHelper::CreateStructuredBuffer(device, &skeleton_srvbuffer, srv_skeleton_data, sizeof(dx::XMFLOAT4X4), srv_skeleton_data.size(), &skeleton_srv);
	DXHelper::BindStructuredBuffer(context, skeleton_srvbuffer, srv_skeleton_data, BONES_SRV_SLOT, ShaderBindFlag::VERTEX, &skeleton_srv);
	DXHelper::CreateBlendState(device, &blendStateOn, &blendStateOff);
	DXHelper::CreateConstBuffer(device, &scene_buffer, &cb_scene, sizeof(cb_scene));

	/* Screenquad shader */
	Shader* screenQuadShader = new Shader;
	screenQuadShader->SetPixelShader("Shaders/ScreenQuad_ps.hlsl");
	screenQuadShader->SetVertexShader("Shaders/ScreenQuad_vs.hlsl");
	screenQuadShader->Compile(device);

	screenQuadMaterial = Material(screenQuadShader);
	screenQuadMaterial.SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, device), 0, ShaderBindFlag::VERTEX);

	/* Screenquad mesh */
	screenQuadMesh = Mesh::CreateScreenQuad(device);
	DShape::Instance().m_Initialize(device);

	//EXEMPEL
	///AddRenderPass(new PSRenderPass(1, L"Shaders/TestPass.hlsl"));
	AddRenderPass(new FogRenderPass(0));
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
			const Material* mat = queue.front().material;
			mat->BindToContext(context);

			while (!queue.empty())
			{
				
				auto item = queue.front();

				

				switch (item.type)
				{
					case RenderItem::Type::Instanced:
						
						DrawRenderItemInstanced(item); break;
					case RenderItem::Type::Grass:
						context->RSSetState(rasterizerStateCullNone);
						DrawRenderItemGrass(item); break;
						context->RSSetState(rasterizerStateCullBack);
					case RenderItem::Type::Skeleton:
						
						DrawRenderItemSkeleton(item); break;

					case RenderItem::Type::Default:
					default:
						
						DrawRenderItem(item);
						break;
				}

				queue.pop();
			}
			mat->UnbindToContext(context);
		}
	}

	// clear queues
	queue.clear();
}

void Renderer::RenderFrame()
{

	LightManager::Instance().UpdateBuffers(context);

	// Temporärt för att ändra skybox texture
	static int ids = 0;
	static float color = 0.0f;

	color += (float)0.005f;
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

	//We need to clear Depth Stencil View as well.//Emil

	size_t bufferIndex = 0;
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRV);

	ClearRenderTarget(midbuffer);
	SetRenderTarget(midbuffer);
  
	context->OMSetDepthStencilState(dss,0);

	context->RSSetState(rasterizerStateCullBack);
	context->OMSetBlendState(blendStateOff, BLENDSTATEMASK, 0xffffffff);
	
	DrawQueueToTarget(opaqueItemQueue);
	DShape::Instance().m_Draw(context);
	context->RSSetState(rasterizerStateCullNone);
	context->OMSetBlendState(blendStateOn, BLENDSTATEMASK, 0xffffffff);
	
	DrawQueueToTarget(transparentItemQueue);
	
	context->OMSetBlendState(blendStateOff, BLENDSTATEMASK, 0xffffffff);
	context->RSSetState(rasterizerStateCullBack);
	
	int index = 0;

	for (auto i = passes.begin(); i < passes.end(); i++)
	{
		if ((*i)->IsEnabled())
		{
			size_t nextBufferIndex = 1 - bufferIndex;
			RenderTexture& previous = (index == 0) ? midbuffer : renderPassSwapBuffers[bufferIndex];
			RenderTexture& next = renderPassSwapBuffers[nextBufferIndex];

			ClearRenderTarget(next);
			SetRenderTarget(next, false);

			GetContext()->PSSetShaderResources(0, 1, &previous.srv);
			
			if ((*i)->Pass(this, previous, next))
				bufferIndex = nextBufferIndex;

			// overkill? Gives the correct result if outside the loop but errors in output
			context->PSSetShaderResources(0, 1, nullSRV);
			index++;
		}
	}

	ClearRenderTarget(backbuffer);
	SetRenderTarget(backbuffer);
	context->PSSetShaderResources(0, 1, &renderPassSwapBuffers[bufferIndex].srv);
	DrawScreenQuad(screenQuadMaterial);
  
	guiManager->DrawAll();
  
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

void Renderer::SetGUIManager(GUIManager* manager)
{
	this->guiManager = manager;
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

void Renderer::DrawGrass(const CameraComponent& camera, const Mesh& mesh, const Material& material, const dx::XMMATRIX& model)
{
	RenderItem item;
	item.type = RenderItem::Type::Grass;
	item.camera = &camera;
	item.mesh = &mesh;
	item.material = &material;
	item.world = model;
	AddItem(item, false);
}

void Renderer::SetRSToCullNone(bool cullNone)
{
	if (cullNone)
	{
		context->RSSetState(rasterizerStateCullNone);
	}
	else
	{
		context->RSSetState(rasterizerStateCullBack);
	}
}

void Renderer::ClearRenderTarget(const RenderTexture& target)
{
	context->ClearRenderTargetView(target.rtv, DEFAULT_BG_COLOR);
	context->ClearDepthStencilView(target.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
}

void Renderer::SetRenderTarget(const RenderTexture& target, bool setDepth)
{
	if (setDepth)
		context->OMSetRenderTargets(1, &target.rtv, target.dsv);
	else
		context->OMSetRenderTargets(1, &target.rtv, NULL);

	context->RSSetViewports(1, &target.viewport);
}

void Renderer::UpdateTime(float time)
{
	cb_scene.time = time;
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
	
	dx::XMMATRIX invProjection = dx::XMMatrixInverse(NULL, item.camera->GetProjectionMatrix());
	dx::XMStoreFloat4x4(&cb_object_data.invProjection, dx::XMMatrixTranspose(invProjection));

	dx::XMMATRIX vp = dx::XMMatrixMultiply(item.camera->GetViewMatrix(), item.camera->GetProjectionMatrix());
	dx::XMStoreFloat4x4(&cb_object_data.vp, dx::XMMatrixTranspose(vp));

	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::PIXEL);

	cb_material_data = item.material->GetMaterialData();
	DXHelper::BindConstBuffer(context, material_cbuffer, &cb_material_data, CB_MATERIAL_SLOT, ShaderBindFlag::PIXEL);

	dx::XMStoreFloat3(&cb_scene.cameraPosition, item.camera->GetOwner()->GetTransform().GetPosition());
	DXHelper::BindConstBuffer(context, scene_buffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);

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
	dx::XMMATRIX invProjection = dx::XMMatrixInverse(NULL, item.camera->GetProjectionMatrix());
	dx::XMStoreFloat4x4(&cb_object_data.invProjection, dx::XMMatrixTranspose(invProjection));


	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::PIXEL);

	cb_material_data = item.material->GetMaterialData();
	DXHelper::BindConstBuffer(context, material_cbuffer, &cb_material_data, CB_MATERIAL_SLOT, ShaderBindFlag::PIXEL);

	dx::XMStoreFloat3(&cb_scene.cameraPosition, item.camera->GetOwner()->GetTransform().GetPosition());
	DXHelper::BindConstBuffer(context, scene_buffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);

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
	dx::XMStoreFloat4x4(&cb_object_data.mvp, mvp);
	dx::XMStoreFloat4x4(&cb_object_data.world,item.world);
	
	dx::XMMATRIX vp = dx::XMMatrixMultiply(item.camera->GetViewMatrix(), item.camera->GetProjectionMatrix());
	dx::XMStoreFloat4x4(&cb_object_data.vp, dx::XMMatrixTranspose(vp));
	dx::XMMATRIX invProjection = dx::XMMatrixInverse(NULL, item.camera->GetProjectionMatrix());
	dx::XMStoreFloat4x4(&cb_object_data.invProjection, dx::XMMatrixTranspose(invProjection));

	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);
	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::PIXEL);


	srv_skeleton_data = *item.bones;
	DXHelper::BindStructuredBuffer(context, skeleton_srvbuffer, srv_skeleton_data, BONES_SRV_SLOT, ShaderBindFlag::VERTEX, &skeleton_srv);

	cb_material_data = item.material->GetMaterialData();
	DXHelper::BindConstBuffer(context, material_cbuffer, &cb_material_data, CB_MATERIAL_SLOT, ShaderBindFlag::PIXEL);
	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	dx::XMStoreFloat3(&cb_scene.cameraPosition, item.camera->GetOwner()->GetTransform().GetPosition());
	DXHelper::BindConstBuffer(context, scene_buffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);

	context->IASetVertexBuffers(0, 1, &item.mesh->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->topology);

	context->DrawIndexed(item.mesh->indices.size(), 0, 0);
}

void Renderer::DrawRenderItemGrass(const RenderItem& item)
{
	
	dx::XMMATRIX mvp = dx::XMMatrixMultiply(item.world, dx::XMMatrixMultiply(item.camera->GetViewMatrix(), item.camera->GetProjectionMatrix()));
	dx::XMStoreFloat4x4(&cb_object_data.mvp,mvp);

	dx::XMMATRIX wv = dx::XMMatrixMultiply(item.world, item.camera->GetViewMatrix());
	dx::XMStoreFloat4x4(&cb_object_data.wv, wv);

	dx::XMStoreFloat4x4(&cb_object_data.world, item.world);

	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::VERTEX);

	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::HULL);

	DXHelper::BindConstBuffer(context, obj_cbuffer, &cb_object_data, CB_OBJECT_SLOT, ShaderBindFlag::GEOMETRY);

	dx::XMStoreFloat3(&cb_scene.cameraPosition, item.camera->GetOwner()->GetTransform().GetPosition());
	DXHelper::BindConstBuffer(context, scene_buffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);
	DXHelper::BindConstBuffer(context, scene_buffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::DOMAINS);
	DXHelper::BindConstBuffer(context, scene_buffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::VERTEX);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &item.mesh->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
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
