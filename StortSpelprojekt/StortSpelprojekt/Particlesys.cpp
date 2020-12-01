#include "stdafx.h"
#include "Particlesys.h"

Particlesys::Particlesys(/*Shader* soShader, Shader* drawShader*/)
	: initializeVB(0), drawVB(0), streamoutVB(0), particleSRV(0), randomNumberSRV(0), inputLayout(0)
{
	////streamoutShader = new Shader;
	//streamoutShader = soShader;
	////drawShader = new Shader;
	//this->drawShader = drawShader;

	firstRun = true;
	gameTimer = 0.0f;
	ageTimeStep = 0.0f;
	particleAge = 0.0f;
	particleMaxAge = 1.0f;
	maxParticles = 200.0f;

	particleColor = dx::XMFLOAT4(1, 1, 1, 1);
	eyePos = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	emitPos = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	emitDir = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
	particleSize = dx::XMFLOAT2(0.1f, 0.1f);
	particleSpreadMulti = dx::XMFLOAT3(0.5f, 1.0f, 0.5f);

	usingTexture = false;

	streamoutVS = L"Shaders/ParticleSO_vs.hlsl";
	drawVS = L"Shaders/ParticleDraw_vs.hlsl";
	streamoutGS = L"Shaders/ParticleSO_gs.hlsl";
	drawGS = L"Shaders/ParticleDraw_gs.hlsl";
	drawPS = L"Shaders/ParticleDraw_ps.hlsl";	
}

Particlesys::~Particlesys()
{
	if (initializeVB) { initializeVB->Release(); }
	if (drawVB) { drawVB->Release(); }
	if (streamoutVB) { streamoutVB->Release(); }
	if (particleSRV) { particleSRV->Release(); }
	if (randomNumberSRV) { randomNumberSRV->Release(); }
}

bool Particlesys::InitializeParticleShaders(ID3D11Device* device, HWND hwnd)
{
	/* VS SHADERS */
	hr = D3DCompileFromFile(streamoutVS, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, &streamoutVSBlob, &ErrorBlob);
	if (FAILED(hr)) {
		if (ErrorBlob) {
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
			MessageBox(hwnd, streamoutVS, L"Missing Shader File", MB_OK);
			ErrorBlob->Release();
			//ReleasePtr(ErrorBlob);
		}

		if (streamoutVSBlob) {
			streamoutVSBlob->Release();
			//ReleasePtr(streamoutVSBlob);
		}
		assert(false);

		return false;
	}

	hr = D3DCompileFromFile(drawVS, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, &drawVSBlob, &ErrorBlob);
	if (FAILED(hr)) {
		if (ErrorBlob) {
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
			MessageBox(hwnd, drawVS, L"Missing Shader File", MB_OK);
			ErrorBlob->Release();
			//ReleasePtr(ErrorBlob);
		}

		if (drawVSBlob) {
			drawVSBlob->Release();
			//ReleasePtr(drawVSBlob);
		}
		assert(false);

		return false;
	}

	/* VS SHADERS END */

	/* GS SHADERS */
	hr = D3DCompileFromFile(streamoutGS, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, &streamoutGSBlob, &ErrorBlob);
	if (FAILED(hr)) {
		if (ErrorBlob) {
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
			MessageBox(hwnd, streamoutGS, L"Missing Shader File", MB_OK);
			ErrorBlob->Release();
			//ReleasePtr(ErrorBlob);
		}

		if (streamoutGSBlob) {
			streamoutGSBlob->Release();
			//ReleasePtr(streamoutGSBlob);
		}
		assert(false);

		return false;
	}

	hr = D3DCompileFromFile(drawGS, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, &drawGSBlob, &ErrorBlob);
	if (FAILED(hr)) {
		if (ErrorBlob) {
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
			MessageBox(hwnd, drawGS, L"Missing Shader File", MB_OK);
			ErrorBlob->Release();
			//ReleasePtr(ErrorBlob);
		}

		if (drawGSBlob) {
			drawGSBlob->Release();
			//ReleasePtr(drawGSBlob);
		}
		assert(false);

		return false;
	}

	/* GS SHADERS END */

	/* PS SHADERS */
	hr = D3DCompileFromFile(drawPS, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, &drawPSBlob, &ErrorBlob);
	if (FAILED(hr)) {
		if (ErrorBlob) {
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
			MessageBox(hwnd, drawPS, L"Missing Shader File", MB_OK);
			ErrorBlob->Release();
			//ReleasePtr(ErrorBlob);
		}

		if (drawPSBlob) {
			drawPSBlob->Release();
			//ReleasePtr(drawPSBlob);
		}
		assert(false);

		return false;
	}


	hr = device->CreateVertexShader(streamoutVSBlob->GetBufferPointer(), streamoutVSBlob->GetBufferSize(), nullptr, &this->streamoutVertexShader);
	if (FAILED(hr))
	{
		return false;
	}

	hr = device->CreateVertexShader(drawVSBlob->GetBufferPointer(), drawVSBlob->GetBufferSize(), nullptr, &this->drawVertexShader);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_SO_DECLARATION_ENTRY soDeclaration[] = {

			{0,"POSITION", 0, 0,3,0},
			{0,"VELOCITY", 0, 0, 3, 0},
			{0,"SIZE", 0, 0, 2, 0},
			{0,"AGE", 0, 0, 1, 0},
			{0,"TYPE", 0, 0, 1, 0}
	};

	UINT stride2 = sizeof(Particle);
	hr = device->CreateGeometryShaderWithStreamOutput(streamoutGSBlob->GetBufferPointer(), streamoutGSBlob->GetBufferSize(), soDeclaration, _countof(soDeclaration), &stride2, 1, 0, NULL,
		&streamoutGeometryShader);
	assert(SUCCEEDED(hr));


	hr = device->CreateGeometryShader(drawGSBlob->GetBufferPointer(), drawGSBlob->GetBufferSize(), nullptr, &this->drawGeometryShader);
	if (FAILED(hr))
	{
		return false;
	}

	hr = device->CreatePixelShader(drawPSBlob->GetBufferPointer(), drawPSBlob->GetBufferSize(), nullptr, &this->drawPixelShader);
	if (FAILED(hr))
	{
		return false;
	}


	/* PS SHADERS END*/

	D3D11_INPUT_ELEMENT_DESC InputLayoutParticle[5] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = device->CreateInputLayout(InputLayoutParticle, ARRAYSIZE(InputLayoutParticle), streamoutVSBlob->GetBufferPointer(), streamoutVSBlob->GetBufferSize(), &inputLayout);
	if (FAILED(hr))
	{
		return false;
	}




	//D3D11_BUFFER_DESC cBufferDescription;
	//// Setup the description of the bufferPerObject constant buffer that is in the vertex shader.
	//ZeroMemory(&cBufferDescription, sizeof(D3D11_BUFFER_DESC));
	//cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	//cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(cBufferPerObject) + (16 - (sizeof(cBufferPerObject) % 16)));
	//cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//cBufferDescription.CPUAccessFlags = 0;
	//cBufferDescription.MiscFlags = 0;
	//cBufferDescription.StructureByteStride = 0;

	//hr = device->CreateBuffer(&cBufferDescription, NULL, &cbufferPerObject);
	//if (FAILED(hr))
	//{
	//	return false;
	//}

	D3D11_BUFFER_DESC cBufferDescription;
	// Setup the description of the camera constant buffer that is in the vertex shader.
	ZeroMemory(&cBufferDescription, sizeof(cBufferDescription));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(cBufferPerFrame) + (16 - (sizeof(cBufferPerFrame) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &cbufferPerFrame);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Particlesys::InitializeParticles(ID3D11Device* device, Renderer* renderer, Object* objectRef)
{
	this->renderer = renderer;
	this->objectRef = objectRef;
	//mMaxParticles = maxParticles;

	/*hr = DirectX::CreateWICTextureFromFile(device, L"Textures/fire1.png", nullptr, &particleSRV);
	assert(SUCCEEDED(hr));*/

	ID3D11Texture1D* random;
	DirectX::XMFLOAT4* randomValues = new DirectX::XMFLOAT4[1024];
	for (int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = RandomFloat(-1, 1);
		randomValues[i].y = RandomFloat(-1, 1);
		randomValues[i].z = RandomFloat(-1, 1);
		randomValues[i].w = RandomFloat(-1, 1);
	}

	bool stop = 0;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024 * sizeof(DirectX::XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	D3D11_TEXTURE1D_DESC texDesc = {};
	texDesc.Width = 1024;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.MipLevels = 1;

	HRESULT hr = device->CreateTexture1D(&texDesc, &initData, &random);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(random, &viewDesc, &randomNumberSRV);
	assert(SUCCEEDED(hr));
	delete[] randomValues;

	/*hr = DirectX::CreateWICTextureFromFile(device, L"Textures/NoiseTextureColor2.jpg", nullptr, &randomSrv);
	assert(SUCCEEDED(hr));	*/

	sampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device);

	BuildVB(device);
}

void Particlesys::SetTexture(ID3D11Device* device, LPCWSTR particleTexture)
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(device, particleTexture, nullptr, &particleSRV);
	assert(SUCCEEDED(hr));

	usingTexture = true;
}

void Particlesys::Reset()
{
	firstRun = true;
	particleAge = 0.0f;
}

void Particlesys::Update(float deltaTime, float gameTime)
{
	gameTimer = gameTime;
	ageTimeStep = deltaTime;

	/* Check some values outside shader */
	particleAge += ageTimeStep;

	dx::XMFLOAT3 pos;
	dx::XMStoreFloat3(&pos, objectRef->GetTransform().GetPosition());
	pos.y += 0.4f;
	pos.z += -0.1f;
	pos.x -= 0.2f;
	SetEmitPos(pos);	
}

void Particlesys::Draw(ID3D11DeviceContext* context, CameraComponent* cam)
{
	dx::XMFLOAT3 eyeCam;
	dx::XMStoreFloat3(&eyeCam, cam->GetOwner()->GetTransform().GetPosition());
	SetEyePos(eyeCam);
	
	renderer->GetContext()->OMSetDepthStencilState(nullptr, 0);
	DrawStreamOut(context, cam);
	renderer->EnableAlphaBlending();
	DrawParticles(context, cam);
	renderer->DisableAlphaBlending();
	renderer->GetContext()->OMSetDepthStencilState(renderer->GetDepthEnable(), 0);
}

float Particlesys::RandomFloat(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void Particlesys::DrawStreamOut(ID3D11DeviceContext* context, CameraComponent* cam)
{
	dx::XMMATRIX viewproj; // = view * projection;
	viewproj = XMMatrixMultiply(cam->GetViewMatrix(), cam->GetProjectionMatrix());

	/* Streamout stuffy  */
	cbPerFrame.emitDir = emitDir; // GetEmitDir();
	cbPerFrame.emitPos = emitPos; // GetEmitPos();
	cbPerFrame.eyePos = eyePos; // GetEyePos();
	cbPerFrame.gameTime = gameTimer;
	cbPerFrame.ageTimeStep = ageTimeStep;
	cbPerFrame.viewProjection = DirectX::XMMatrixTranspose(viewproj);
	cbPerFrame.particleMaxAge = particleMaxAge;
	cbPerFrame.particleColor = particleColor;
	cbPerFrame.usingTexture = usingTexture;
	cbPerFrame.particleSpreadMulti = particleSpreadMulti;
	cbPerFrame.particlesPerSecond = particlesPerSecond;

	context->UpdateSubresource(cbufferPerFrame, 0, nullptr, &cbPerFrame, 0, 0);
	context->GSSetConstantBuffers(0, 1, &cbufferPerFrame);

	context->IASetInputLayout(inputLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->GSSetShaderResources(0, 1, &randomNumberSRV);
	context->GSSetSamplers(0, 1, &sampler);

	//streamoutShader->BindToContext(context);

	context->VSSetShader(streamoutVertexShader, 0, 0);
	context->GSSetShader(streamoutGeometryShader, 0, 0);
	context->PSSetShader(nullptr, 0, 0);


	UINT stride = sizeof(Particle);
	UINT offset = 0;

	if (firstRun)
		context->IASetVertexBuffers(0, 1, &initializeVB, &stride, &offset);
	else
		context->IASetVertexBuffers(0, 1, &drawVB, &stride, &offset);

	context->SOSetTargets(1, &streamoutVB, &offset);

	if (firstRun)
	{
		context->Draw(1, 0);
		firstRun = false;
	}
	else
	{
		context->DrawAuto();
	}

	// Ping-pong the vertex buffers
	ID3D11Buffer* bufferArray[1] = { 0 };
	context->SOSetTargets(1, bufferArray, &offset);

	std::swap(drawVB, streamoutVB);

	/* Clear */
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	context->GSSetShaderResources(0, 1, nullSRV);

	ID3D11SamplerState* nullSampler[1] = { nullptr };
	context->GSSetSamplers(0, 1, nullSampler);

	//streamoutShader->Unbind(context);
	context->GSSetConstantBuffers(0, 1, bufferArray);
	context->VSSetShader(nullptr, 0, 0);
	context->GSSetShader(nullptr, 0, 0);
	context->PSSetShader(nullptr, 0, 0);
}

void Particlesys::DrawParticles(ID3D11DeviceContext* context, CameraComponent* cam)
{	
	UINT stride = sizeof(Particle);
	UINT offset = 0;

	context->UpdateSubresource(cbufferPerFrame, 0, nullptr, &cbPerFrame, 0, 0);
	context->VSSetConstantBuffers(0, 1, &cbufferPerFrame);
	context->GSSetConstantBuffers(0, 1, &cbufferPerFrame);
	context->PSSetConstantBuffers(0, 1, &cbufferPerFrame);

	context->PSSetShaderResources(0, 1, &particleSRV);
	// Set the sampler state in the pixel shader.
	context->PSSetSamplers(0, 1, &sampler);

	context->IASetVertexBuffers(0, 1, &drawVB, &stride, &offset);
	// Sets pixel and vertex shader
	context->VSSetShader(drawVertexShader, 0, 0);
	context->GSSetShader(drawGeometryShader, 0, 0);
	context->PSSetShader(drawPixelShader, 0, 0);

	//drawShader->BindToContext(context);

	context->DrawAuto();

	ID3D11Buffer* bufferArray[1] = { 0 };
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRV);

	ID3D11SamplerState* nullSampler[1] = { nullptr };
	context->PSSetSamplers(0, 1, nullSampler);

	context->GSSetConstantBuffers(0, 1, bufferArray);
	context->GSSetConstantBuffers(1, 1, bufferArray);
	context->VSSetConstantBuffers(0, 1, bufferArray);
	context->VSSetConstantBuffers(1, 1, bufferArray);
	context->PSSetConstantBuffers(0, 1, bufferArray);

	context->VSSetShader(nullptr, 0, 0);
	context->GSSetShader(nullptr, 0, 0);
	context->PSSetShader(nullptr, 0, 0); 
}

void Particlesys::BuildVB(ID3D11Device* device)
{	
	D3D11_BUFFER_DESC vbDesc;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(Particle) * 1;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	Particle p;
	ZeroMemory(&p, sizeof(Particle));
	p.age = 0.0f;
	p.type = 0;
	p.size = particleSize;
	p.startVelocity = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	p.startPos = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &p;

	HRESULT hr = (device->CreateBuffer(&vbDesc, &vinitData, &initializeVB));
	assert(SUCCEEDED(hr));

	// Ping-pong buffers?? for stream-out and drawing	
	vbDesc.ByteWidth = sizeof(Particle) * maxParticles;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	hr = (device->CreateBuffer(&vbDesc, 0, &drawVB));
	assert(SUCCEEDED(hr));
	hr = (device->CreateBuffer(&vbDesc, 0, &streamoutVB));
	assert(SUCCEEDED(hr));
}
