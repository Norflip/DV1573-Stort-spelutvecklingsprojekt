#include "stdafx.h"
#include "Renderer.h"
#include "RenderPass.h"
#include "DShape.h"
#include "Input.h"


Renderer::Renderer() : device(nullptr), context(nullptr), swapchain(nullptr), skeleton_srvbuffer(nullptr), skeleton_srv(nullptr), batchInstanceBuffer(nullptr)
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
	

	delete[] tmpBatchInstanceData;
	RELEASE(o_LightGrid_tex);
	RELEASE(o_LightGrid_texSRV);
	RELEASE(t_LightGrid_tex);
	RELEASE(t_LightGrid_texSRV);
	RELEASE(frustums_buffer);
	RELEASE(inFrustums_srv);
	RELEASE(outFrustums_uav);
	RELEASE(o_LightIndexCounter_uavbuffer);
	RELEASE(o_LightIndexCounter_uav);
	RELEASE(t_LightIndexCounter_uavbuffer);
	RELEASE(t_LightIndexCounter_uav);
	RELEASE(o_LightIndexList_uavbuffer);
	RELEASE(o_LightIndexList_srv);
	RELEASE(o_LightIndexList_uav);
	RELEASE(t_LightIndexList_uavbuffer);
	RELEASE(t_LightIndexList_srv);
	RELEASE(t_LightIndexList_uav);
	

}

void Renderer::Initialize(Window* window)
{
	this->outputWindow = window;

	DXHelper::CreateSwapchain(*window, &device, &context, &swapchain);
	this->backbuffer = DXHelper::CreateBackbuffer(window->GetWidth() , window->GetHeight(), device, swapchain);
	this->midbuffer = DXHelper::CreateRenderTexture(window->GetWidth() , window->GetHeight(), device, context, &dss);
	this->renderPassSwapBuffers[0] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device, context, &dss);
	this->renderPassSwapBuffers[1] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device, context, &dss);
	srv_skeleton_data.resize(60);



	DXHelper::CreateRSState(device, &rasterizerStateCullBack, &rasterizerStateCullNone, &rasterizerStateCCWO);




	LightManager::Instance().Initialize(device);


	sceneBuffer.Initialize(CB_SCENE_SLOT, ShaderBindFlag::PIXEL | ShaderBindFlag::DOMAINS | ShaderBindFlag::VERTEX|ShaderBindFlag::COMPUTE, device);
	objectBuffer.Initialize(CB_OBJECT_SLOT, ShaderBindFlag::VERTEX | ShaderBindFlag::DOMAINS|ShaderBindFlag::GEOMETRY, device);
	materialBuffer.Initialize(CB_MATERIAL_SLOT, ShaderBindFlag::PIXEL, device);

	DXHelper::CreateStructuredBuffer(device, &skeleton_srvbuffer, srv_skeleton_data.data(), sizeof(dx::XMFLOAT4X4), srv_skeleton_data.size(), &skeleton_srv);
	DXHelper::BindStructuredBuffer(context, skeleton_srvbuffer, srv_skeleton_data.data(), BONES_SRV_SLOT, ShaderBindFlag::VERTEX, &skeleton_srv);
	DXHelper::CreateBlendState(device, &blendStateOn, &blendStateOff);


	/* Screenquad shader */
	Shader* screenQuadShader = new Shader;
	//screenQuadShader->SetPixelShader("Shaders/ScreenQuad_ps.hlsl");
	//screenQuadShader->SetVertexShader("Shaders/ScreenQuad_vs.hlsl");
	screenQuadShader->Compile(device);

	screenQuadMaterial = new Material(screenQuadShader);
	screenQuadMaterial->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, device), 0, ShaderBindFlag::VERTEX);

	/* Screenquad mesh */
	screenQuadMesh = CreateScreenQuad();
	DShape::Instance().m_Initialize(device);

	//	CreateInstanceBuffer(device, MAX_BATCH_COUNT, )

	tmpBatchInstanceData = new dx::XMFLOAT4X4[MAX_BATCH_COUNT];
	DXHelper::CreateInstanceBuffer(device, MAX_BATCH_COUNT, sizeof(dx::XMFLOAT4X4), tmpBatchInstanceData, &batchInstanceBuffer);
}


void Renderer::DrawQueueToTarget(RenderQueue& queue, CameraComponent* camera)
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
						DrawRenderItemInstanced(item, camera); break;

					case RenderItem::Type::Grass:
						DrawRenderItemGrass(item, camera); break;

					case RenderItem::Type::Skeleton:
						DrawRenderItemSkeleton(item, camera); break;

					case RenderItem::Type::Particles:
						DrawRenderItemParticles(item, camera); break;

					case RenderItem::Type::Default:
					default:

						DrawRenderItem(item, camera);
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


void Renderer::RenderFrame(CameraComponent* camera, float time, float distance)
{
	// UPDATE SCENE
	
	/*if (KEY_PRESSED(Y) && isFullScreen == true)
	{
		isFullScreen = false;
	}
	else if (KEY_PRESSED(Y) && isFullScreen == false)
	{
		isFullScreen = true;
	}*/
	RenderFrame(camera, time, distance, backbuffer, true, true);
	HRESULT hr = swapchain->Present(0, 0); //1 here?
	//swapchain->SetFullscreenState(isFullScreen, nullptr);
	assert(SUCCEEDED(hr));
}

void Renderer::RenderFrame(CameraComponent* camera, float time, float distance, RenderTexture& target, bool drawGUI, bool applyRenderPasses)
{


	// UPPDATERA SCENE
	// ----------

	// Tempor�rt f�r att �ndra skybox texture
	//ids = 0;
	//color = 0.0f;

	//color += (float)0.005f;
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
	data.distanceToHouse = distance;
	//std::cout << xPos << std::endl;
	//std::cout << yPos << std::endl;
	xPos += (float)Input::Instance().GetPrevMousePosRelative().y;
	yPos += (float)Input::Instance().GetPrevMousePosRelative().x;
	data.mousePos = { xPos,yPos };
	data.screenSize = { (float)outputWindow->GetWidth(), (float)outputWindow->GetHeight() };
	//data.mousePos = { (float)Input::Instance().GetMousePos().x, (float)Input::Instance().GetMousePos().y };
	// put in mouse pos delta here
	dx::XMStoreFloat3(&data.cameraPosition, camera->GetOwner()->GetTransform().GetPosition());
	dx::XMStoreFloat4x4(&data.invProjection, dx::XMMatrixTranspose(dx::XMMatrixInverse(NULL, camera->GetProjectionMatrix())));
	dx::XMStoreFloat4x4(&data.invView, dx::XMMatrixTranspose(dx::XMMatrixInverse(NULL, camera->GetViewMatrix())));
	dx::XMStoreFloat4x4(&data.view, dx::XMMatrixTranspose(camera->GetViewMatrix()));
	sceneBuffer.SetData(data);
	sceneBuffer.UpdateBuffer(context);

	// ----------



	//LightManager::Instance().UpdateBuffers(context,camera);

	UpdateForwardPlus(camera);

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRV);

	ClearRenderTarget(midbuffer);
	SetRenderTarget(midbuffer);

	for (auto i = passes.begin(); i < passes.end(); i++)
	{
		RenderPass* pass = *i;
		if (pass->IsEnabled() && pass->GetType() == RenderPass::PassType::SKYBOX)
		{
			pass->Pass(this, camera, renderPassSwapBuffers[0], renderPassSwapBuffers[0]);
		}
	}
	

	context->OMSetDepthStencilState(dss, 0);
	DXHelper::BindStructuredBuffer(context, 10, ShaderBindFlag::PIXEL, &o_LightIndexList_srv);
	context->PSSetShaderResources(11, 1, &o_LightGrid_texSRV);
	SetCullBack(true);
	DrawQueueToTarget(opaqueItemQueue, camera);
	DShape::Instance().m_Draw(camera->GetViewMatrix() * camera->GetProjectionMatrix(), context);

	for (auto i : opaqueBatches)
		DrawBatch(i.second, camera);
	
	opaqueBatches.clear();
	DXHelper::BindStructuredBuffer(context, 10, ShaderBindFlag::PIXEL, &t_LightIndexList_srv);
	context->PSSetShaderResources(11, 1, &t_LightGrid_texSRV);
	SetCullBack(false);
	DrawQueueToTarget(transparentItemQueue, camera);
	for (auto i : transparentBatches)
		DrawBatch(i.second, camera);

	transparentBatches.clear();

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

				pass->Pass(this, camera, previous, passTarget);
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
			if (pass->IsEnabled() && pass->GetType() == RenderPass::PassType::GUI)
			{
				pass->Pass(this, camera, renderPassSwapBuffers[0], renderPassSwapBuffers[0]);
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


void Renderer::Draw(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model, bool batchable)
{
	if (batchable)
	{
		int batchID = GetBatchID(material, mesh);
		auto& batches = material->IsTransparent() ? transparentBatches : opaqueBatches;
		dx::XMFLOAT4X4 modelInFloats;
		dx::XMStoreFloat4x4(&modelInFloats, dx::XMMatrixTranspose(model));

		if (batches.find(batchID) != batches.end())
		{
			batches[batchID].transformations.push_back(modelInFloats);
		}
		else
		{
			Batch batch;
			batch.material = material;
			batch.mesh = mesh;
			batch.transformations.push_back(modelInFloats);
			batches.insert({ batchID, batch });
		}

		auto& batchesDepth = material->IsTransparent() ? transparentBatchesDepth : opaqueBatchesDepth;
		if (batchesDepth.find(batchID) != batchesDepth.end())
		{
			batchesDepth[batchID].transformations.push_back(modelInFloats);
		}
		else
		{
			Batch batch;
			batch.material = material;
			batch.mesh = mesh;
			batch.transformations.push_back(modelInFloats);
			batchesDepth.insert({ batchID, batch });
		}
	}
	else
	{
		RenderItem item;
		item.mesh = mesh;
		item.material = material;
		item.type = RenderItem::Type::Default;
		item.world = model;
		AddItem(item, material->IsTransparent(), false);
	}
}

void Renderer::DrawInstanced(const Mesh* mesh, const size_t& count, ID3D11Buffer* instanceBuffer, const Material* material)
{
	RenderItem item;
	item.mesh = mesh;
	item.material = material;
	item.type = RenderItem::Type::Instanced;
	item.world = dx::XMMatrixIdentity();
	item.instanceBuffer = instanceBuffer;
	item.instanceCount = count;

	AddItem(item, material->IsTransparent(), false);
}

void Renderer::DrawSkeleton(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model, std::vector<dx::XMFLOAT4X4>& bones)
{
	RenderItem item;
	item.mesh = mesh;
	item.material = material;
	item.type = RenderItem::Type::Skeleton;
	item.bones = &bones;
	item.world = model;
	AddItem(item, false, false);
}

void Renderer::DrawGrass(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model)
{
	RenderItem item;
	item.type = RenderItem::Type::Grass;
	item.mesh = mesh;
	item.material = material;
	item.world = model;
	AddItem(item, false, true);
}

void Renderer::DrawParticles(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model)
{
	RenderItem part;
	part.type = RenderItem::Type::Particles;
	part.mesh = mesh;
	part.material = material;
	part.world = model;
	AddItem(part, true, true);
}

void Renderer::DrawImmediate(const Mesh* mesh, const Material* material, const CameraComponent* camera, const dx::XMMATRIX& model)
{
	SetObjectBufferValues(camera, model, true);
	objectBuffer.UpdateBuffer(context);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(mesh->GetTopology());

	context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
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
		//context->OMSetBlendState(blendStateOff, BLENDSTATEMASK, 0xffffffff);
		context->OMSetBlendState(blendStateOn, BLENDSTATEMASK, 0xffffffff);
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



void Renderer::SetIdAndColor(int id, float color)
{
	this->ids = id;
	this->color = color;
}

void Renderer::AddItem(const RenderItem& item, bool transparent, bool cullDepth)
{
	if (transparent)
	{
		size_t materialID = item.material->GetID();
		auto found = transparentItemQueue.find(materialID);

		if (found == transparentItemQueue.end())
			transparentItemQueue.insert({ materialID, std::queue<RenderItem>() });

		transparentItemQueue[materialID].push(item);
		if (!cullDepth)
		{
			auto foundDepth = transparentItemQueueDepth.find(materialID);
			if (foundDepth == transparentItemQueueDepth.end())
				transparentItemQueueDepth.insert({ materialID, std::queue<RenderItem>() });
			transparentItemQueueDepth[materialID].push(item);
		}
		
	}
	else
	{
		size_t materialID = item.material->GetID();
		auto found = opaqueItemQueue.find(materialID);

		if (found == opaqueItemQueue.end())
			opaqueItemQueue.insert({ materialID, std::queue<RenderItem>() });

		opaqueItemQueue[materialID].push(item);
		if (!cullDepth)
		{
			auto foundDepth = opaqueItemQueueDepth.find(materialID);
			if (foundDepth == opaqueItemQueueDepth.end())
				opaqueItemQueueDepth.insert({ materialID, std::queue<RenderItem>() });
			opaqueItemQueueDepth[materialID].push(item);
		}
	}
}

void Renderer::DrawRenderItem(const RenderItem& item, CameraComponent* camera)
{
	SetObjectBufferValues(camera, item.world, true);
	objectBuffer.UpdateBuffer(context);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = item.mesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->GetTopology());

	context->DrawIndexed(item.mesh->GetIndexCount(), 0, 0);
}

void Renderer::DrawRenderItemInstanced(const RenderItem& item, CameraComponent* camera)
{
	SetObjectBufferValues(camera, item.world, true);
	objectBuffer.UpdateBuffer(context);

	UINT stride[2] = { sizeof(Mesh::Vertex), sizeof(dx::XMFLOAT4X4) };
	UINT offset[2] = { 0 };

	ID3D11Buffer* buffers[2];
	buffers[0] = item.mesh->GetVertexBuffer();
	buffers[1] = item.instanceBuffer;

	context->IASetVertexBuffers(0, 2, buffers, stride, offset);
	context->IASetIndexBuffer(item.mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->GetTopology());

	context->DrawIndexedInstanced(item.mesh->GetIndexCount(), item.instanceCount, 0, 0, 0);
}

void Renderer::DrawRenderItemSkeleton(const RenderItem& item, CameraComponent* camera)
{
	SetObjectBufferValues(camera, item.world, true);
	objectBuffer.UpdateBuffer(context);

	srv_skeleton_data = *item.bones;
	DXHelper::BindStructuredBuffer(context, skeleton_srvbuffer, srv_skeleton_data.data(), BONES_SRV_SLOT, ShaderBindFlag::VERTEX, &skeleton_srv);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = item.mesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->GetTopology());

	context->DrawIndexed(item.mesh->GetIndexCount(), 0, 0);
}

void Renderer::DrawRenderItemGrass(const RenderItem& item, CameraComponent* camera)
{
	SetObjectBufferValues(camera, item.world, false);
	objectBuffer.UpdateBuffer(context);

	ShaderBindFlag def = objectBuffer.GetFlag();
	objectBuffer.SetBindFlag(def | ShaderBindFlag::VERTEX | ShaderBindFlag::HULL | ShaderBindFlag::GEOMETRY);
	objectBuffer.UpdateBuffer(context);
	objectBuffer.SetBindFlag(def);

	context->RSSetState(rasterizerStateCullNone);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = item.mesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(item.mesh->GetIndexCount(), 0, 0);
	context->RSSetState(rasterizerStateCullBack);
}

void Renderer::DrawRenderItemParticles(const RenderItem& item, CameraComponent* camera)
{
	SetObjectBufferValues(camera, item.world, true);
	objectBuffer.UpdateBuffer(context);

	UINT stride = sizeof(Mesh::VertexColor);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = item.mesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->GetTopology());
	//SetCBuffers(context, camera);
	//particlesShader->BindToContext(context);

	//context->Draw(item.mesh->GetVertexCount(), 0);
	context->DrawIndexed(item.mesh->GetIndexCountPart(), 0, 0);
}

void Renderer::DrawBatch(const Batch& batch, CameraComponent* camera)
{
	// Skapa indexbuffer

	size_t instanceCount = std::min(batch.transformations.size(), UICAST(MAX_BATCH_COUNT));
	if (instanceCount == 0)
		return;

	SetObjectBufferValues(camera, dx::XMMatrixIdentity(), true);
	objectBuffer.UpdateBuffer(context);


	D3D11_MAPPED_SUBRESOURCE mappedData;
	ZeroMemory(&mappedData, sizeof(D3D11_MAPPED_SUBRESOURCE));

	GetContext()->Map(batchInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	dx::XMFLOAT4X4* dataView = reinterpret_cast<dx::XMFLOAT4X4*>(mappedData.pData);

	for (size_t i = 0; i < batch.transformations.size(); i++)
	{
		dataView[i] = batch.transformations[i];
	}

	GetContext()->Unmap(batchInstanceBuffer, 0);



	UINT stride[2] = { sizeof(Mesh::Vertex), sizeof(dx::XMFLOAT4X4) };
	UINT offset[2] = { 0 };

	ID3D11Buffer* buffers[2];
	buffers[0] = batch.mesh->GetVertexBuffer();
	buffers[1] = batchInstanceBuffer;

	context->IASetVertexBuffers(0, 2, buffers, stride, offset);
	context->IASetIndexBuffer(batch.mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(batch.mesh->GetTopology());

	context->DrawIndexedInstanced(batch.mesh->GetIndexCount(), instanceCount, 0, 0, 0);

	//std::cout << "BATCHING(" << batch.material->IsTransparent() << "): " << batch.mesh->GetMeshName() << " / " << batch.material->GetID() << " : " << instanceCount << "\n";
}

void Renderer::SetObjectBufferValues(const CameraComponent* camera, dx::XMMATRIX world, bool transpose)
{
	if (transpose)
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
	else
	{
		cb_Object& data = objectBuffer.GetData();
		dx::XMMATRIX view = camera->GetViewMatrix();

		dx::XMMATRIX wv = dx::XMMatrixMultiply(world, view);
		dx::XMStoreFloat4x4(&data.wv, wv);

		dx::XMMATRIX vp = dx::XMMatrixMultiply(view, camera->GetProjectionMatrix());
		dx::XMStoreFloat4x4(&data.vp, vp);

		dx::XMMATRIX mvp = dx::XMMatrixMultiply(world, vp);
		dx::XMStoreFloat4x4(&data.mvp, mvp);
		dx::XMStoreFloat4x4(&data.world, world);
		objectBuffer.SetData(data);
	}
}

Mesh* Renderer::CreateScreenQuad()
{
	const float size = 1.0f;
	std::vector<Mesh::Vertex> vertices =
	{
		Mesh::Vertex{{-size, -size, 0} , {0 ,1 },  {0,0,0} , { 0,0,0 } },		// 0,0
		Mesh::Vertex{{ size, -size, 0 }, { 1,1 }, { 0,0,0 }, { 0,0,0 }},		// 0, w
		Mesh::Vertex{{ size, size, 0 }, { 1,0 }, { 0,0,0 }, { 0,0,0 }},		// h, w
		Mesh::Vertex{{ -size, size, 0 }, { 0,0 }, { 0,0,0 }, { 0,0,0 }}		// h, 0
	};

	std::vector<unsigned int> indices = { 3,2,1, 3,1,0 };

	Mesh* quad = new Mesh(vertices, indices);
	quad->Initialize(device);
	return quad;
}

void Renderer::DrawScreenQuad(const Material* material)
{

	material->BindToContext(context);
	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = screenQuadMesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(screenQuadMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(screenQuadMesh->GetIndexCount(), 0, 0);
}


void Renderer::InitForwardPlus(CameraComponent* camera, Window* window, Shader& forwardPlusShader)
{
	
	
	this->width = window->GetWidth();
	this->height = window->GetHeight();
	int screenWidth = std::max(window->GetWidth(), 1u);
	int screenHeight = std::max(window->GetHeight(), 1u);
	int lightCullingBlockSize = 32;
	this->numThreads = dx::XMUINT3(UICAST(std::ceil((float)screenWidth / (float)lightCullingBlockSize)), UICAST(std::ceil((float)screenHeight / (float)lightCullingBlockSize)), 1);
	this->numThreadGroups = dx::XMUINT3(UICAST(std::ceil((float)numThreads.x / (float)lightCullingBlockSize)), UICAST(std::ceil((float)numThreads.y / (float)lightCullingBlockSize)), 1);
	UINT count = numThreads.x * numThreads.y * numThreads.z;

	//Dispatch Forward+
	dispatchParamsBuffer.Initialize(CB_DISPATCH_PARAMS_SLOT, ShaderBindFlag::COMPUTE, device);
	cb_DispatchParams& dataDP = dispatchParamsBuffer.GetData();
	dataDP.numThreadGroups = dx::XMUINT4(numThreadGroups.x, numThreadGroups.y, numThreadGroups.z, 1);
	dataDP.numThreads = dx::XMUINT4(numThreads.x, numThreads.y, numThreads.z, 1);
	dispatchParamsBuffer.SetData(dataDP);
	dispatchParamsBuffer.UpdateBuffer(context);

	//ScreenToViewParams Forward+
	screenToViewParams.Initialize(CB_SCREEN_TOVIEW_PARAMS_SLOT, ShaderBindFlag::COMPUTE, device);
	cb_ScreenToViewParams& dataSVP = screenToViewParams.GetData();
	dx::XMFLOAT4X4 inverse;
	dx::XMStoreFloat4x4(&inverse, dx::XMMatrixTranspose(dx::XMMatrixInverse(nullptr, camera->GetProjectionMatrix())));
	dataSVP.inverseProjection = inverse;
	dataSVP.screenDimensions.x = FCAST(window->GetWidth());
	dataSVP.screenDimensions.y = FCAST(window->GetHeight());
	screenToViewParams.SetData(dataSVP);
	screenToViewParams.UpdateBuffer(context);

	//out_Frustums Forward+
	frustum_data.resize(count);
	DXHelper::CreateCopyBuffer(device, &frustums_buffer, sizeof(s_Frustum), frustum_data.size());
	DXHelper::CreateStructuredBuffer(device, &frustums_buffer, frustum_data.data(), sizeof(s_Frustum), frustum_data.size(), &outFrustums_uav, &inFrustums_srv);
	DXHelper::BindStructuredBuffer(context, frustums_buffer, frustum_data.data(), 0, ShaderBindFlag::COMPUTE, &outFrustums_uav, nullptr); //u0

	context->Dispatch(numThreads.x, numThreads.y, numThreads.z);
	//context->Dispatch(numThreadGroups.x, numThreadGroups.y, numThreadGroups.z);
	//context->Dispatch(1, 1, 1);

	//D3D11_MAPPED_SUBRESOURCE resource;
	//HRESULT hr = context->Map(frustums_buffer, 0, D3D11_MAP_READ_WRITE, 0, &resource);
	//assert(SUCCEEDED(hr));
	//s_Frustum* someFrustums;
	//someFrustums = (s_Frustum*)resource.pData;
	//
	//for (int i = 0; i < count; i++)
	//{
	//	frustum_data[i] = someFrustums[i];
	//}
	//context->Unmap(frustums_buffer, 0);

	ID3D11Texture2D* tex2D = nullptr;
	ID3D11Texture2D* tex2D2 = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};

	desc.Width = numThreads.x;
	desc.Height = numThreads.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32_UINT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	device->CreateTexture2D(&desc, NULL, &tex2D);
	device->CreateTexture2D(&desc, NULL, &tex2D2);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32G32_UINT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32_UINT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;

	device->CreateUnorderedAccessView(tex2D, &uavDesc, &o_LightGrid_tex);
	device->CreateUnorderedAccessView(tex2D2, &uavDesc, &t_LightGrid_tex);
	device->CreateShaderResourceView(tex2D, &srvDesc, &o_LightGrid_texSRV);
	device->CreateShaderResourceView(tex2D, &srvDesc, &t_LightGrid_texSRV);
	tex2D->Release();
	tex2D2->Release();

	forwardPlusShader.Unbind(context);
	forwardPlusShader.SetComputeShader("Shaders/ForwardPlusRendering.hlsl");
	forwardPlusShader.CompileCS(device);
	forwardPlusShader.BindToContext(context);

	
	//opaque_light index counter
	
	o_LightIndexCounter.push_back(0);
	DXHelper::CreateStructuredBuffer(device, &o_LightIndexCounter_uavbuffer, o_LightIndexCounter.data(), sizeof(UINT), o_LightIndexCounter.size(), &o_LightIndexCounter_uav);
	
	//transparent_light index counter
	
	t_LightIndexCounter.push_back(0);
	DXHelper::CreateStructuredBuffer(device, &t_LightIndexCounter_uavbuffer, t_LightIndexCounter.data(), sizeof(UINT), t_LightIndexCounter.size(), &t_LightIndexCounter_uav);
	
	//avarage overlapping lights per tile = 200
	//int indexList = numThreadGroups.x * numThreadGroups.y * numThreadGroups.z * 40;
	int indexList = count * 40;
	o_LightIndexList.resize(indexList); //count *30 
	t_LightIndexList.resize(indexList);
	/*for (int index = 0; index < 32; index++)
	{
		o_LightIndexList[index] = 0;
		t_LightIndexList[index] = 0;
	}*/
	DXHelper::CreateStructuredBuffer(device, &o_LightIndexList_uavbuffer, o_LightIndexList.data(), sizeof(UINT), o_LightIndexList.size(), &o_LightIndexList_uav, &o_LightIndexList_srv);
	
	DXHelper::CreateStructuredBuffer(device, &t_LightIndexList_uavbuffer, t_LightIndexList.data(), sizeof(UINT), t_LightIndexList.size(), &t_LightIndexList_uav, &t_LightIndexList_srv);
	depthPass.Init(device, width,height);
	
	
}

void Renderer::UpdateForwardPlus(CameraComponent* camera)
{
	//this is to be run for lightculling compute shader
	//////DEPTH PASS BEGIN---------------------------
	
	depthPass.BindNull(context);
	depthPass.BindDSV(context);
	context->OMSetDepthStencilState(dss, 0);
	SetCullBack(true);
	DrawQueueToTarget(opaqueItemQueueDepth, camera);
	
	for (auto i : opaqueBatchesDepth)
		DrawBatch(i.second, camera);

	opaqueBatchesDepth.clear();

	SetCullBack(false);
	DrawQueueToTarget(transparentItemQueueDepth, camera);
	for (auto i : transparentBatchesDepth)
		DrawBatch(i.second, camera);

	transparentBatchesDepth.clear();

	SetCullBack(true);

	depthPass.BindNull(context);
	//////DEPTH PASS END-----------------------------------------------
	ClearRenderTarget(midbuffer);
	SetRenderTarget(midbuffer);
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL); //frustum
	context->CSSetUnorderedAccessViews(3, 1, &nullUAV, NULL); //u3
	context->CSSetUnorderedAccessViews(4, 1, &nullUAV, NULL); //u4
	context->CSSetUnorderedAccessViews(5, 1, &nullUAV, NULL); //u5
	context->CSSetUnorderedAccessViews(6, 1, &nullUAV, NULL); //u6
	context->PSSetShaderResources(10, 1, &nullSRV);
	context->PSSetShaderResources(11, 1, &nullSRV);
	context->OMSetDepthStencilState(dss, 0);
	context->CSSetShaderResources(1, 1, depthPass.GetDepthSRV());
	DXHelper::BindStructuredBuffer(context, 9, ShaderBindFlag::COMPUTE, &inFrustums_srv);
	LightManager::Instance().UpdateBuffers(context, camera);
	o_LightIndexCounter[0] = 0;
	t_LightIndexCounter[0] = 0;
	DXHelper::BindStructuredBuffer(context, o_LightIndexCounter_uavbuffer, o_LightIndexCounter.data(), 1, ShaderBindFlag::COMPUTE, &o_LightIndexCounter_uav, nullptr); //u1
	DXHelper::BindStructuredBuffer(context, t_LightIndexCounter_uavbuffer, t_LightIndexCounter.data(), 2, ShaderBindFlag::COMPUTE, &t_LightIndexCounter_uav, nullptr); //u2
	DXHelper::BindStructuredBuffer(context, o_LightIndexList_uavbuffer, o_LightIndexList.data(), 3, ShaderBindFlag::COMPUTE, &o_LightIndexList_uav, nullptr); //u3
	DXHelper::BindStructuredBuffer(context, t_LightIndexList_uavbuffer, t_LightIndexList.data(), 4, ShaderBindFlag::COMPUTE, &t_LightIndexList_uav, nullptr); //u4

	context->CSSetUnorderedAccessViews(5, 1, &o_LightGrid_tex, NULL); //u5
	context->CSSetUnorderedAccessViews(6, 1, &t_LightGrid_tex, NULL); //u6
	
	context->Dispatch(numThreads.x, numThreads.y, numThreads.z);
	//context->Dispatch(numThreadGroups.x, numThreadGroups.y, numThreadGroups.z);
	context->CSSetUnorderedAccessViews(3, 1, &nullUAV, NULL); //u3
	context->CSSetUnorderedAccessViews(4, 1, &nullUAV, NULL); //u4
	context->CSSetUnorderedAccessViews(5, 1, &nullUAV, NULL); //u5
	context->CSSetUnorderedAccessViews(6, 1, &nullUAV, NULL); //u6
	context->PSSetShaderResources(10, 1, &nullSRV);
	context->PSSetShaderResources(11, 1, &nullSRV);
	
	//context->Dispatch(1, 1, 1);
}

