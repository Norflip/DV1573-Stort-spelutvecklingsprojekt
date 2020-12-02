#include "Rain.h"

void Rain::m_Initialize(ID3D11Device* device)
{
	emitterDataBuffer.Initialize(0, ShaderBindFlag::COMPUTE, device);

	auto& data = emitterDataBuffer.GetData();
	data.time = 0.0f;
	data.lifeTime = MAX_LIFETIME;
	data.emitterPosition = { 0,0,0 };
	emitterDataBuffer.SetData(data);

	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags = flags | D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* csBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT cs_result = D3DCompileFromFile(L"Shaders/Rain_cs.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", flags, 0, &csBlob, &errorBlob);

	if (FAILED(cs_result))
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	cs_result = device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &computeShader);
	assert(SUCCEEDED(cs_result));

	// SHADER
	shader = new Shader();
	shader->SetVertexShader("Shaders/Rain_vs.hlsl");
	shader->SetPixelShader("Shaders/Rain_ps.hlsl");
	shader->SetGeometryShader("Shaders/Rain_gs.hlsl");
	shader->SetInputLayoutStructure(1, shader->RAIN_INPUT_LAYOUT);
	shader->Compile(device);

	material = Material(shader);

	particlePositions.clear();

	for (size_t i = 0; i < MAX_PARTICLES; i++)
	{
		ParticleInput pd;
		pd.startPosition = RandomPointOnDisc(dx::XMFLOAT3(0, 0, 0), 10.0f, 10.0f);
		pd.timeOffset = (static_cast<float>(rand() % 1000) / 1000.0f) * MAX_LIFETIME;
		pd.speed = -15.0f;
		particleData.push_back(pd);

		ParticleOutput particle;
		particle.position = dx::XMFLOAT3(0, 0, -10);
		particlePositions.push_back(particle);
	}

	InitializeBuffers(device);
}

void Rain::Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& inTexture, RenderTexture& outTexture)
{
	material.BindToContext(renderer->GetContext());
	renderer->UpdateObjectBuffer(camera, dx::XMMatrixIdentity(), ShaderBindFlag::GEOMETRY);

	ID3D11DeviceContext* context = renderer->GetContext();
	UpdatePositions(camera->GetOwner()->GetTransform().GetPosition(), context);

	UINT stride = sizeof(ParticleOutput);
	UINT offset = 0;
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->Draw(MAX_PARTICLES, 0);

	material.UnbindToContext(renderer->GetContext());
}

void Rain::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDescription;

	// VERTEX BUFFER
	ZeroMemory(&vertexBufferDescription, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDescription.ByteWidth = sizeof(ParticleOutput) * particlePositions.size();
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	HRESULT vertexBufferResult = device->CreateBuffer(&vertexBufferDescription, 0, &vertexBuffer);
	assert(SUCCEEDED(vertexBufferResult));

	HRESULT hr;

	// INPUT
	D3D11_BUFFER_DESC inputBufferDescription;
	ZeroMemory(&inputBufferDescription, sizeof(D3D11_BUFFER_DESC));
	inputBufferDescription.ByteWidth = sizeof(ParticleInput) * MAX_PARTICLES;
	inputBufferDescription.StructureByteStride = sizeof(ParticleInput);
	inputBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	inputBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	inputBufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	inputBufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA subresourceData;
	ZeroMemory(&subresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	subresourceData.pSysMem = particleData.data();
	hr = device->CreateBuffer(&inputBufferDescription, &subresourceData, &particleDataBuffer);

	// INPUT VIEW
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = MAX_PARTICLES;
	hr = device->CreateShaderResourceView(particleDataBuffer, &srvDesc, &inputView);

	// OUTPUT
	D3D11_BUFFER_DESC outputDesc;
	ZeroMemory(&outputDesc, sizeof(D3D11_BUFFER_DESC));
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.ByteWidth = sizeof(ParticleOutput) * MAX_PARTICLES;
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.CPUAccessFlags = 0;
	outputDesc.StructureByteStride = sizeof(ParticleOutput);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(&outputDesc, 0, &outputBuffer);

	// OUTPUT ACCESS
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = MAX_PARTICLES;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	hr = device->CreateUnorderedAccessView(outputBuffer, &uavDesc, &outputUAV);
}

void Rain::UpdatePositions(dx::XMVECTOR eye, ID3D11DeviceContext* context)
{
	GameClock& clock = GameClock::Instance();

	auto& data = emitterDataBuffer.GetData();
	dx::XMStoreFloat3(&data.emitterPosition, eye);
	data.emitterPosition.y += Y_OFFSET;
	data.time = clock.GetSeconds();
	
	emitterDataBuffer.SetData(data);
	emitterDataBuffer.UpdateBuffer(context);
	
	context->CSSetShader(computeShader, 0, 0);
	context->CSSetShaderResources(0, 1, &inputView);
	context->CSSetUnorderedAccessViews(0, 1, &outputUAV, 0);

	// 100 * threadgroups(10 * 1 * 1) = 1000
	context->Dispatch(100, 1, 1);

	//// Unbind the input textures from the CS for good housekeeping
	ID3D11ShaderResourceView* nullSRV[] = { NULL };
	context->CSSetShaderResources(0, 1, nullSRV);

	// Unbind output from compute shader
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


	context->CopyResource(vertexBuffer, outputBuffer);
}

dx::XMFLOAT3 Rain::RandomPointOnDisc(dx::XMFLOAT3 center, float height, float radius)
{
	const int precision = pow(10, 4);
	float value0 = ((rand() % precision) / (float)precision);
	float value1 = ((rand() % precision) / (float)precision);

	float angle = value0 * 2 * Math::PI;

	float offset = value1 * radius;
	float x = cosf(angle) * offset;
	float z = sinf(angle) * offset;
	
	return dx::XMFLOAT3(center.x + x, center.y, center.z + z);
}
