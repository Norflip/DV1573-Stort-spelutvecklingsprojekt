#include "Renderer.h"
#include "RenderPass.h"
#include "FogRenderPass.h"
#include "DShape.h"

Renderer::Renderer() : device(nullptr), context(nullptr), swapchain(nullptr), skeleton_srvbuffer(nullptr), skeleton_srv(nullptr)
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

	for (auto i = passes.begin(); i < passes.end(); i++)
		delete (*i);

	skeleton_srvbuffer->Release();
	rasterizerStateCullBack->Release();
	rasterizerStateCullNone->Release();

	skeleton_srv->Release();
	dss->Release();
	rasterizerStateCCWO->Release();
}

void Renderer::Initialize(Window* window)
{
	this->outputWindow = window;

	DXHelper::CreateSwapchain(*window, &device, &context, &swapchain);
	this->backbuffer = DXHelper::CreateBackbuffer(window->GetWidth(), window->GetHeight(), device, swapchain);
	this->midbuffer = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device, context, &dss);
	this->renderPassSwapBuffers[0] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device, context, &dss);
	this->renderPassSwapBuffers[1] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device, context, &dss);
	srv_skeleton_data.resize(60);

	dx::XMFLOAT4X4 bone;
	dx::XMStoreFloat4x4(&bone, dx::XMMatrixIdentity());

	DXHelper::CreateRSState(device, &rasterizerStateCullBack, &rasterizerStateCullNone, &rasterizerStateCCWO);


	for (int boneNr = 0; boneNr < 60; boneNr++) //set id matrix as default for the bones. So if no animation is happening the character is not funky.
	{
		srv_skeleton_data[boneNr] = bone;//set id matrix as default for the bones. So if no animation is happening the character is not funky. I need to bind them as well.
	}

	LightManager::Instance().Initialize(device);


	sceneBuffer.Initialize(CB_SCENE_SLOT, ShaderBindFlag::PIXEL | ShaderBindFlag::DOMAINS | ShaderBindFlag::VERTEX, device);
	objectBuffer.Initialize(CB_OBJECT_SLOT, ShaderBindFlag::VERTEX, device);
	materialBuffer.Initialize(CB_MATERIAL_SLOT, ShaderBindFlag::PIXEL, device);

	DXHelper::CreateStructuredBuffer(device, &skeleton_srvbuffer, srv_skeleton_data, sizeof(dx::XMFLOAT4X4), srv_skeleton_data.size(), &skeleton_srv);
	DXHelper::BindStructuredBuffer(context, skeleton_srvbuffer, srv_skeleton_data, BONES_SRV_SLOT, ShaderBindFlag::VERTEX, &skeleton_srv);
	DXHelper::CreateBlendState(device, &blendStateOn, &blendStateOff);


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
			materialBuffer.SetData(mat->GetMaterialData());
			materialBuffer.UpdateBuffer(context);

			while (!queue.empty())
			{
				auto item = queue.front();

				switch (item.type)
				{
					case RenderItem::Type::Instanced:
						DrawRenderItemInstanced(item); break;

					case RenderItem::Type::Grass:
						DrawRenderItemGrass(item); break;

					case RenderItem::Type::Skeleton:
						context->RSSetState(rasterizerStateCCWO);
						DrawRenderItemSkeleton(item); break;
						context->RSSetState(rasterizerStateCullBack);
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


void Renderer::RenderFrame(CameraComponent* camera, float time)
{
	// UPDATE SCENE
	RenderFrame(camera, time, backbuffer, true, true);
	HRESULT hr = swapchain->Present(0, 0); //1 here?
	assert(SUCCEEDED(hr));
}

void Renderer::RenderFrame(CameraComponent* camera, float time, RenderTexture& target, bool drawGUI, bool applyRenderPasses)
{

	// UPPDATERA SCENE
	// ----------

	// Tempor�rt f�r att �ndra skybox texture
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

	cb_Scene& data = sceneBuffer.GetData();

	data.id = ids;
	data.factor = color;
	data.time = time;
	dx::XMStoreFloat3(&data.cameraPosition, camera->GetOwner()->GetTransform().GetPosition());
	dx::XMStoreFloat4x4(&data.invProjection, dx::XMMatrixTranspose(dx::XMMatrixInverse(NULL, camera->GetProjectionMatrix())));
	dx::XMStoreFloat4x4(&data.invView, dx::XMMatrixTranspose(dx::XMMatrixInverse(NULL, camera->GetViewMatrix())));

	sceneBuffer.SetData(data);
	sceneBuffer.UpdateBuffer(context);

	// ----------


	LightManager::Instance().UpdateBuffers(context);


	//We need to clear Depth Stencil View as well.//Emil

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRV);

	ClearRenderTarget(midbuffer);
	SetRenderTarget(midbuffer);

	// ADD LATER WITH FOG
	//ID3D11ShaderResourceView* depthSRV = renderPassSwapBuffers[bufferIndex].depthSRV;

	context->OMSetDepthStencilState(dss, 0);


	SetCullBack(true);
	DrawQueueToTarget(opaqueItemQueue);
	DShape::Instance().m_Draw(context);

	SetCullBack(false);
	DrawQueueToTarget(transparentItemQueue);

	SetCullBack(true);
	size_t passCount = 0;
	size_t bufferIndex = 0;

	if (applyRenderPasses)
	{
		for (auto i = passes.begin(); i < passes.end(); i++)
		{
			RenderPass* pass = *i;
			if (pass->IsEnabled() && pass->GetType() == RenderPass::PassType::POST_PROCESSING)
			{
				size_t nextBufferIndex = 1 - bufferIndex;
				RenderTexture& passTarget = renderPassSwapBuffers[nextBufferIndex];
				RenderTexture& previous = (passCount == 0) ? midbuffer : renderPassSwapBuffers[bufferIndex];

				pass->Pass(this, previous, passTarget);
				if (pass->GetType() == RenderPass::PassType::POST_PROCESSING)
					bufferIndex = nextBufferIndex;

				// overkill? Gives the correct result if outside the loop but errors in output
				//context->PSSetShaderResources(0, 1, nullSRV);
				passCount++;
			}
		}
	}

	RenderTexture& lastBuffer = (passCount == 0) ? midbuffer : renderPassSwapBuffers[bufferIndex];
	ClearRenderTarget(target);
	SetRenderTarget(target, false);

	context->PSSetShaderResources(0, 1, &lastBuffer.srv);
	DrawScreenQuad(screenQuadMaterial);

	if (drawGUI)
	{
		for (auto i = passes.begin(); i < passes.end(); i++)
		{
			RenderPass* pass = *i;
			if (pass->IsEnabled() && pass->GetType() == RenderPass::PassType::UI_OVERLAY)
			{
				pass->Pass(this, renderPassSwapBuffers[0], renderPassSwapBuffers[0]);
			}
		}
	}
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

void Renderer::SetCullBack(bool cullNone)
{
	if (!cullNone)
	{
		context->RSSetState(rasterizerStateCullNone);
		context->OMSetBlendState(blendStateOn, BLENDSTATEMASK, 0xffffffff);
	}
	else
	{
		context->RSSetState(rasterizerStateCullBack);
		context->OMSetBlendState(blendStateOff, BLENDSTATEMASK, 0xffffffff);
	}
}

void Renderer::ClearRenderTarget(const RenderTexture& target, bool clearDepth)
{
	context->ClearRenderTargetView(target.rtv, DEFAULT_BG_COLOR);
	if (clearDepth)
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

void Renderer::RemoveRenderPass(RenderPass* pass)
{
	std::vector<RenderPass*>::iterator it = std::find(passes.begin(), passes.end(), pass);

	passes.erase(it);
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
	SetObjectBufferValues(item.camera, item.world);
	objectBuffer.UpdateBuffer(context);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &item.mesh->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->topology);

	context->DrawIndexed(item.mesh->indices.size(), 0, 0);
}

void Renderer::DrawRenderItemInstanced(const RenderItem& item)
{
	SetObjectBufferValues(item.camera, item.world);
	objectBuffer.UpdateBuffer(context);

	UINT stride[2] = { sizeof(Mesh::Vertex), sizeof(Mesh::InstanceData) };
	UINT offset[2] = { 0 };

	context->IASetVertexBuffers(0, 2, item.mesh->vertexAndInstance, stride, offset);
	context->IASetIndexBuffer(item.mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->topology);
	context->DrawIndexedInstanced(item.mesh->indices.size(), item.instanceCount, 0, 0, 0);
}

void Renderer::DrawRenderItemSkeleton(const RenderItem& item)
{
	SetObjectBufferValues(item.camera, item.world);
	objectBuffer.UpdateBuffer(context);

	srv_skeleton_data = *item.bones;
	DXHelper::BindStructuredBuffer(context, skeleton_srvbuffer, srv_skeleton_data, BONES_SRV_SLOT, ShaderBindFlag::VERTEX, &skeleton_srv);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &item.mesh->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->topology);

	context->DrawIndexed(item.mesh->indices.size(), 0, 0);
}

void Renderer::DrawRenderItemGrass(const RenderItem& item)
{
	SetObjectBufferValues(item.camera, item.world);
	objectBuffer.UpdateBuffer(context);

	ShaderBindFlag def = objectBuffer.GetFlag();
	objectBuffer.SetBindFlag(def | ShaderBindFlag::VERTEX | ShaderBindFlag::HULL | ShaderBindFlag::GEOMETRY);
	objectBuffer.UpdateBuffer(context);
	objectBuffer.SetBindFlag(def);

	context->RSSetState(rasterizerStateCullNone);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &item.mesh->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(item.mesh->indices.size(), 0, 0);
	context->RSSetState(rasterizerStateCullBack);
}

void Renderer::SetObjectBufferValues(const CameraComponent* camera, dx::XMMATRIX world)
{
	cb_Object& data = objectBuffer.GetData();
	dx::XMMATRIX view = camera->GetViewMatrix();

	dx::XMMATRIX wv = dx::XMMatrixMultiply(world, view);
	dx::XMStoreFloat4x4(&data.wv, dx::XMMatrixTranspose(wv));

	dx::XMMATRIX vp = dx::XMMatrixMultiply(view, camera->GetProjectionMatrix());
	dx::XMStoreFloat4x4(&data.vp, dx::XMMatrixTranspose(vp));

	dx::XMMATRIX mvp = dx::XMMatrixMultiply(world, vp);
	dx::XMStoreFloat4x4(&data.mvp, dx::XMMatrixTranspose(mvp));
	dx::XMStoreFloat4x4(&data.world, dx::XMMatrixTranspose(world));
	objectBuffer.SetData(data);
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
