#include "stdafx.h"
#include "ParticleComponent.h"

ParticleComponent::ParticleComponent(Renderer* renderer, Shader* shader)
    : initializeVB(nullptr), drawVB(nullptr), streamoutVB(nullptr), particleSRV(nullptr), randomNumberSRV(nullptr)//, inputLayout(0)
{
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

	/* Benchmark */
	particlesPerSecond = maxParticles / 2;
}

ParticleComponent::~ParticleComponent()
{
	if (initializeVB) { initializeVB->Release(); }
	if (drawVB) { drawVB->Release(); }
	if (streamoutVB) { streamoutVB->Release(); }
	if (particleSRV) { particleSRV->Release(); }
	if (randomNumberSRV) { randomNumberSRV->Release(); }
}

void ParticleComponent::InitializeParticles(ID3D11Device* device)
{
	ID3D11Texture1D* random;
	DirectX::XMFLOAT4* randomValues = new DirectX::XMFLOAT4[1024];
	for (int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = RandomFloat(-1, 1);
		randomValues[i].y = RandomFloat(-1, 1);
		randomValues[i].z = RandomFloat(-1, 1);
		randomValues[i].w = RandomFloat(-1, 1);
	}

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


	D3D11_BUFFER_DESC cBufferDescription;
	// Setup the description of the camera constant buffer that is in the vertex shader.
	ZeroMemory(&cBufferDescription, sizeof(cBufferDescription));
	cBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	cBufferDescription.ByteWidth = static_cast<uint32_t>(sizeof(cb_particle) + (16 - (sizeof(cb_particle) % 16)));
	cBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDescription.CPUAccessFlags = 0;
	cBufferDescription.MiscFlags = 0;
	cBufferDescription.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDescription, NULL, &cb_Per_Particle);
	assert(SUCCEEDED(hr));	


	BuildVertexBuffers(device);
}

void ParticleComponent::Update(const float& deltaTime)
{
	gameTimer = (float)GameClock::Instance().GetSeconds(); //   gameTime;
	ageTimeStep = deltaTime;

	/* Check some values outside shader */
	particleAge += ageTimeStep;
}

void ParticleComponent::Draw(Renderer* renderer, CameraComponent* camera)
{



}

void ParticleComponent::SetTexture(ID3D11Device* device, LPCWSTR particleTexture)
{
	hr = DirectX::CreateWICTextureFromFile(device, particleTexture, nullptr, &particleSRV);
	assert(SUCCEEDED(hr));

	usingTexture = true;
}

void ParticleComponent::Reset()
{
	firstRun = true;
	particleAge = 0.0f;
}

float ParticleComponent::RandomFloat(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void ParticleComponent::BuildVertexBuffers(ID3D11Device* device)
{
	// Create the buffer to kick-off the particle system.
	D3D11_BUFFER_DESC vbDesc;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(Mesh::Particle) * 1;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	Mesh::Particle p;
	ZeroMemory(&p, sizeof(Mesh::Particle));
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
	vbDesc.ByteWidth = sizeof(Mesh::Particle) * maxParticles;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	hr = (device->CreateBuffer(&vbDesc, 0, &drawVB));
	assert(SUCCEEDED(hr));
	hr = (device->CreateBuffer(&vbDesc, 0, &streamoutVB));
	assert(SUCCEEDED(hr));
}

void ParticleComponent::DrawStreamOut(ID3D11DeviceContext* context, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, CameraComponent* cam, ID3D11SamplerState* sampler)
{
}

void ParticleComponent::Draw(ID3D11DeviceContext* context, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, CameraComponent* cam, ID3D11SamplerState* sampler)
{
}
