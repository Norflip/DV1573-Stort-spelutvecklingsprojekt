#include "stdafx.h"
#include "FireTextureComponent.h"

FireTextureComponent::FireTextureComponent(Renderer* renderer, Shader* shader)
{
	this->renderer = renderer;
	this->fireShader = shader;
	//this->meshMaterial = new Material(fireShader);
	this->quadMatrix = dx::XMMatrixIdentity();

	quadMaterial = ZWEBLoader::LoadMaterials("Models/Quad.ZWEB", fireShader, renderer->GetDevice());
	quadMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/Quad.ZWEB", renderer->GetDevice());
	bool stop = 0;

	fireObject = new Object("fire", ObjectFlag::DEFAULT | ObjectFlag::NO_CULL);
	fireObject->AddComponent<MeshComponent>(quadMesh[0], quadMaterial[0]);	
}

FireTextureComponent::~FireTextureComponent()
{
}

void FireTextureComponent::InitializeFire(ID3D11Device* device)
{
	InitializeBuffers(device);
	LoadTextures(device);
}

void FireTextureComponent::Shutdown()
{
}

void FireTextureComponent::Update(const float& deltaTime)
{
	UpdateBuffers();
	dx::XMFLOAT3 firePosition;
	firePosition.x = GetOwner()->GetTransform().GetPosition().m128_f32[0];
	firePosition.y = GetOwner()->GetTransform().GetPosition().m128_f32[1] + 0.2f;
	firePosition.z = GetOwner()->GetTransform().GetPosition().m128_f32[2];

	dx::XMMATRIX fireTranslation = dx::XMMatrixTranslation(firePosition.x, firePosition.y, firePosition.z);
	dx::XMMATRIX worldParticles = dx::XMMatrixIdentity();
	worldParticles = fireTranslation;
	quadMatrix = worldParticles;
}

void FireTextureComponent::Draw(Renderer* renderer, CameraComponent* camera)
{
	renderer->Draw(quadMesh[0], quadMaterial[0], quadMatrix, false);
}

void FireTextureComponent::LoadTextures(ID3D11Device* device)
{
	HRESULT hr = dx::CreateWICTextureFromFile(device, L"Textures/fireAlpha.png", nullptr, &alphaTexture);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load WIC Texture'", L"Graphics scene Initialization Message", MB_ICONERROR);

	hr = dx::CreateWICTextureFromFile(device, L"Textures/fireTexture.png", nullptr, &fireTexture);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load WIC Texture'", L"Graphics scene Initialization Message", MB_ICONERROR);

	hr = dx::CreateWICTextureFromFile(device, L"Textures/fireNoise.png", nullptr, &noiseTexture);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load WIC Texture'", L"Graphics scene Initialization Message", MB_ICONERROR);

	/* Mat info */
	quadMaterial[0]->SetTexture(new Texture(fireTexture), TEXTURE_DIFFUSE_SLOT, ShaderBindFlag::PIXEL);
	quadMaterial[0]->SetTexture(new Texture(noiseTexture), TEXTURE_DIFFUSE2_SLOT, ShaderBindFlag::PIXEL);
	quadMaterial[0]->SetTexture(new Texture(alphaTexture), TEXTURE_DIFFUSE3_SLOT, ShaderBindFlag::PIXEL);
	quadMaterial[0]->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device), 0, ShaderBindFlag::PIXEL);
	quadMaterial[0]->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, device), 0, ShaderBindFlag::PIXEL);
	quadMaterial[0]->SetTransparent(true);
}

void FireTextureComponent::InitializeBuffers(ID3D11Device* device)
{	
	// Setup the description of the dynamic noise constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC noiseBufferDesc;
	noiseBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	noiseBufferDesc.ByteWidth = sizeof(cb_Noise);
	noiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseBufferDesc.CPUAccessFlags = 0;
	noiseBufferDesc.MiscFlags = 0;
	noiseBufferDesc.StructureByteStride = 0;

	HRESULT result = device->CreateBuffer(&noiseBufferDesc, NULL, &noiseBuffer);
	assert(SUCCEEDED(result));

	// Setup the description of the dynamic distortion constant buffer that is in the pixel shader.
	D3D11_BUFFER_DESC distortionBufferDesc;
	distortionBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	distortionBufferDesc.ByteWidth = sizeof(cb_Disortion);
	distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBufferDesc.CPUAccessFlags = 0;
	distortionBufferDesc.MiscFlags = 0;
	distortionBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&distortionBufferDesc, NULL, &disortionBuffer);
	assert(SUCCEEDED(result));		
}

void FireTextureComponent::UpdateBuffers()
{
	dx::XMFLOAT3 scrollSpeeds;
	dx::XMFLOAT3 scales;
	dx::XMFLOAT2 distortion1;
	dx::XMFLOAT2 distortion2; 
	dx::XMFLOAT2 distortion3;
	float distortionScale;
	float distortionBias;
	
	// Increment the frame time counter.
	static float frameTime = 0.0f;
	frameTime += 0.01f;
	if (frameTime > 1000.0f)
	{
		frameTime = 0.0f;
	}

	// Set the three scrolling speeds for the three different noise textures.
	scrollSpeeds = dx::XMFLOAT3(1.3f, 2.1f, 2.3f);

	// Set the three scales which will be used to create the three different noise octave textures.
	scales = dx::XMFLOAT3(1.0f, 2.0f, 3.0f);

	// Set the three different x and y distortion factors for the three different noise textures.
	distortion1 = dx::XMFLOAT2(0.1f, 0.2f);
	distortion2 = dx::XMFLOAT2(0.1f, 0.3f);
	distortion3 = dx::XMFLOAT2(0.1f, 0.1f);

	// The the scale and bias of the texture coordinate sampling perturbation.
	distortionScale = 0.8f;
	distortionBias = 0.5f;

	noise.frameTime = frameTime;
	noise.scrollSpeeds = scrollSpeeds;
	noise.scales = scales;
	noise.padding = 0.0f;

	renderer->GetContext()->UpdateSubresource(noiseBuffer, 0, nullptr, &noise, 0, 0);
	renderer->GetContext()->VSSetConstantBuffers(1, 1, &noiseBuffer);
	
	disortion.distortion1 = distortion1;
	disortion.distortion2 = distortion2;
	disortion.distortion3 = distortion3;
	disortion.distortionScale = distortionScale;
	disortion.distortionBias = distortionBias;

	renderer->GetContext()->UpdateSubresource(disortionBuffer, 0, nullptr, &disortion, 0, 0);
	renderer->GetContext()->PSSetConstantBuffers(0, 1, &disortionBuffer);
}
