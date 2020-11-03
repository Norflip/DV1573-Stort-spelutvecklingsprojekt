#include "stdafx.h"
#include "Renderer.h"
#include "RenderPass.h"
#include "FogRenderPass.h"
#include "FXAARenderPass.h"
#include "ShadowPass.h"
#include "DShape.h"
#include "Input.h"

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
	rasterizerStateCCWO->Release();
}

void Renderer::Initialize(Window* window)
{
	this->outputWindow = window;

	DXHelper::CreateSwapchain(*window, &device, &context, &swapchain);
	this->backbuffer = DXHelper::CreateBackbuffer(window->GetWidth(), window->GetHeight(), device, swapchain);
	this->midbuffer = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device);
	this->renderPassSwapBuffers[0] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device);
	this->renderPassSwapBuffers[1] = DXHelper::CreateRenderTexture(window->GetWidth(), window->GetHeight(), device);
	srv_skeleton_data.resize(60);

	context->OMSetDepthStencilState(midbuffer.dss, 1);

	dx::XMFLOAT4X4 bone;
	dx::XMStoreFloat4x4(&bone, dx::XMMatrixIdentity());

	DXHelper::CreateRSState(device, &rasterizerStateCullBack, &rasterizerStateCullNone, &rasterizerStateCCWO, &rasterizerStateCullFront);


	for (int boneNr = 0; boneNr < 60; boneNr++) //set id matrix as default for the bones. So if no animation is happening the character is not funky.
	{
		srv_skeleton_data[boneNr] = bone;//set id matrix as default for the bones. So if no animation is happening the character is not funky. I need to bind them as well.
	}

	LightManager::Instance().Initialize(device);


	sceneBuffer.Initialize(CB_SCENE_SLOT, ShaderBindFlag::PIXEL | ShaderBindFlag::DOMAINS | ShaderBindFlag::VERTEX, device);
	objectBuffer.Initialize(CB_OBJECT_SLOT, ShaderBindFlag::VERTEX, device);
	materialBuffer.Initialize(CB_MATERIAL_SLOT, ShaderBindFlag::PIXEL, device);

	DXHelper::CreateStructuredBuffer(device, &skeleton_srvbuffer, srv_skeleton_data.data(), sizeof(dx::XMFLOAT4X4), srv_skeleton_data.size(), &skeleton_srv);
	DXHelper::BindStructuredBuffer(context, skeleton_srvbuffer, srv_skeleton_data.data(), BONES_SRV_SLOT, ShaderBindFlag::VERTEX, &skeleton_srv);
	DXHelper::CreateBlendState(device, &blendStateOn, &blendStateOff);


	/* Screenquad shader */
	Shader* screenQuadShader = new Shader;
	screenQuadShader->SetPixelShader("Shaders/ScreenQuad_ps.hlsl");
	screenQuadShader->SetVertexShader("Shaders/ScreenQuad_vs.hlsl");
	screenQuadShader->Compile(device);

	screenQuadMaterial = new Material(screenQuadShader);
	screenQuadMaterial->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, device), 0, ShaderBindFlag::VERTEX);

	/* Screenquad mesh */
	screenQuadMesh = CreateScreenQuad();
	DShape::Instance().m_Initialize(device);

	//EXEMPEL
	///AddRenderPass(new PSRenderPass(1, L"Shaders/TestPass.hlsl"));
	AddRenderPass(new ShadowDepthPrePass(0));
	
	//AddRenderPass(new FogRenderPass(0));
	AddRenderPass(new ShadowRenderPass(1));
	//AddRenderPass(new FXAARenderPass(10));

}

void Renderer::DrawQueue(VirtualCamera* camera, dx::XMMATRIX cameraTransform, RenderQueue& queue)
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
						RenderInstanced(item, camera, cameraTransform); 
						break;

					case RenderItem::Type::Grass:
						RenderGrass(item, camera, cameraTransform); 
						break;

					case RenderItem::Type::Skeleton:
						RenderSkeleton(item, camera, cameraTransform); 
						break;

					case RenderItem::Type::Default:
					default:
						RenderDefault(item, camera, cameraTransform);
						break;
				}

				queue.pop_front();
			}

			mat->UnbindToContext(context);
		}
	}

	// clear queues
	queue.clear();
}

void Renderer::DrawQueueForPass(VirtualCamera* camera, dx::XMMATRIX cameraTransform, RenderQueue& queue)
{
	for (auto i : queue)
	{
		const Material* mat = i.second.begin()->material;

		mat->BindToContext(context);
		materialBuffer.SetData(mat->GetMaterialData());
		materialBuffer.UpdateBuffer(context);

		for (auto j = i.second.begin(); j < i.second.end(); j++)
		{
			RenderItem& item = *j;


			switch (item.type)
			{
				case RenderItem::Type::Instanced:
					RenderInstanced(item, camera, cameraTransform);
					break;

				case RenderItem::Type::Grass:
					RenderGrass(item, camera, cameraTransform);
					break;

				case RenderItem::Type::Skeleton:
					RenderSkeleton(item, camera, cameraTransform);
					break;

				case RenderItem::Type::Default:
				default:
					RenderDefault(item, camera, cameraTransform);
					break;
			}
		}

		mat->UnbindToContext(context);
	}
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

	sceneBuffer.SetData(data);
	sceneBuffer.UpdateBuffer(context);

	// ----------
	

	LightManager::Instance().UpdateBuffers(data.cameraPosition, context);

	//We need to clear Depth Stencil View as well.//Emil

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRV);

	for (auto i = passes.begin(); i < passes.end(); i++)
	{
		RenderPass* pass = *i;
		if (pass->IsEnabled() && pass->GetType() == RenderPass::PassType::PRE_PASS)
		{
			pass->Pass(this, renderPassSwapBuffers[0], renderPassSwapBuffers[0]);
		}
	}

	dx::XMMATRIX cameraTransform = camera->GetOwner()->GetTransform().GetWorldMatrix();

	ClearRenderTarget(midbuffer);
	SetRenderTarget(midbuffer);

	context->OMSetDepthStencilState(midbuffer.dss, 0);

	SetCullMode(D3D11_CULL_BACK);
	DrawQueue(camera, cameraTransform, opaqueItemQueue);
	DShape::Instance().m_Draw(context);

	SetCullMode(D3D11_CULL_NONE);
	DrawQueue(camera, cameraTransform, transparentItemQueue);

	SetCullMode(D3D11_CULL_BACK);
	size_t passCount = 0;
	size_t bufferIndex = 0;

	if (applyRenderPasses)
	{
		for (auto i = passes.begin(); i < passes.end(); i++)
		{
			RenderPass* pass = *i;
			if (pass->IsEnabled() && pass->GetType() == RenderPass::PassType::POST_PASS)
			{
				size_t nextBufferIndex = 1 - bufferIndex;
				RenderTexture& passTarget = renderPassSwapBuffers[nextBufferIndex];
				RenderTexture& previous = (passCount == 0) ? midbuffer : renderPassSwapBuffers[bufferIndex];

				pass->Pass(this, previous, passTarget);
				if (pass->GetType() == RenderPass::PassType::POST_PASS)
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
			if (pass->IsEnabled() && pass->GetType() == RenderPass::PassType::OVERLAY)
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

void Renderer::Draw(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model)
{
	RenderItem item;
	item.mesh = mesh;
	item.material = material;
	item.type = RenderItem::Type::Default;
	item.world = model;
	AddItem(item, material->IsTransparent());
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

	AddItem(item, material->IsTransparent());
}

void Renderer::DrawSkeleton(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model, std::vector<dx::XMFLOAT4X4>& bones)
{

	RenderItem item;
	item.mesh = mesh;
	item.material = material;
	item.type = RenderItem::Type::Skeleton;
	item.bones = &bones;
	item.world = model;
	AddItem(item, false);

}

void Renderer::DrawGrass(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model)
{
	RenderItem item;
	item.type = RenderItem::Type::Grass;
	item.mesh = mesh;
	item.material = material;
	item.world = model;
	AddItem(item, false);
}

void Renderer::SetCullMode(D3D11_CULL_MODE mode)
{
	switch (mode)
	{
		case D3D11_CULL_NONE:
			context->RSSetState(rasterizerStateCullNone);
			break;
		case D3D11_CULL_FRONT:
			context->RSSetState(rasterizerStateCullFront);
			break;
		case D3D11_CULL_BACK:
			context->RSSetState(rasterizerStateCullBack);
			break;
		default:
			break;
	}

	context->OMSetBlendState(blendStateOn, BLENDSTATEMASK, 0xffffffff);
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
	{
		context->OMSetRenderTargets(1, &target.rtv, target.dsv);
		context->OMSetDepthStencilState(target.dss, 1);
	}
	else
	{
		context->OMSetRenderTargets(1, &target.rtv, NULL);
	}
		
	context->RSSetViewports(1, &target.viewport);
}

void Renderer::RemoveRenderTarget()
{
	ID3D11RenderTargetView* rtvNull[1] = { nullptr };
	context->OMSetRenderTargets(1, rtvNull, NULL);
}

void Renderer::RemoveRenderPass(RenderPass* pass)
{
	std::vector<RenderPass*>::iterator it = std::find(passes.begin(), passes.end(), pass);

	passes.erase(it);
}

void Renderer::SaveShaderResourceView(std::string key, ID3D11ShaderResourceView* srv)
{
	auto find = registredShaderResourceViews.find(key);
	if (find != registredShaderResourceViews.end())
		registredShaderResourceViews.insert({ key, srv });
	else
		registredShaderResourceViews[key] = srv;
}

ID3D11ShaderResourceView* Renderer::LoadShaderResourceView(std::string key)
{
	ID3D11ShaderResourceView* srv = nullptr;
	auto find = registredShaderResourceViews.find(key);
	if (find != registredShaderResourceViews.end())
		srv = find->second;
	return srv;
}

void Renderer::AddItem(const RenderItem& item, bool transparent)
{
	if (transparent)
	{
		size_t materialID = item.material->GetID();
		auto found = transparentItemQueue.find(materialID);

		if (found == transparentItemQueue.end())
			transparentItemQueue.insert({ materialID, std::deque<RenderItem>() });

		transparentItemQueue[materialID].push_front(item);
	}
	else
	{
		size_t materialID = item.material->GetID();
		auto found = opaqueItemQueue.find(materialID);

		if (found == opaqueItemQueue.end())
			opaqueItemQueue.insert({ materialID, std::deque<RenderItem>() });

		opaqueItemQueue[materialID].push_front(item);
	}
}

void Renderer::RenderDefault(const RenderItem& item, VirtualCamera* camera, dx::XMMATRIX cameraTransform)
{
	SetObjectBufferValues(camera, cameraTransform, item.world, true);
	objectBuffer.UpdateBuffer(context);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = item.mesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(item.mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->GetTopology());

	context->DrawIndexed(item.mesh->GetIndexCount(), 0, 0);
}

void Renderer::RenderInstanced(const RenderItem& item, VirtualCamera* camera, dx::XMMATRIX cameraTransform)
{
	SetObjectBufferValues(camera, cameraTransform, item.world, true);
	objectBuffer.UpdateBuffer(context);

	UINT stride[2] = { sizeof(Mesh::Vertex), sizeof(Mesh::InstanceData) };
	UINT offset[2] = { 0 };

	ID3D11Buffer* buffers[2];
	buffers[0] = item.mesh->GetVertexBuffer();
	buffers[1] = item.instanceBuffer;

	context->IASetVertexBuffers(0, 2, buffers, stride, offset);
	context->IASetIndexBuffer(item.mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(item.mesh->GetTopology());

	context->DrawIndexedInstanced(item.mesh->GetIndexCount(), item.instanceCount, 0, 0, 0);
}

void Renderer::RenderSkeleton(const RenderItem& item, VirtualCamera* camera, dx::XMMATRIX cameraTransform)
{
	SetObjectBufferValues(camera, cameraTransform, item.world, true);
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

void Renderer::RenderGrass(const RenderItem& item, VirtualCamera* camera, dx::XMMATRIX cameraTransform)
{
	SetObjectBufferValues(camera, cameraTransform, item.world, false);
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
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Renderer::SetObjectBufferValues(const VirtualCamera* camera, dx::XMMATRIX cameraTransform, dx::XMMATRIX world, bool transpose)
{
	if (transpose)
	{
		cb_Object& data = objectBuffer.GetData();
		dx::XMMATRIX view = camera->GetViewMatrix(cameraTransform);

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
		dx::XMMATRIX view = camera->GetViewMatrix(cameraTransform);

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


	material->UnbindToContext(context);
}
