#include "stdafx.h"
#include "ParticlesystemComponent.h"

ParticleSystemComponent::ParticleSystemComponent(Renderer* renderer, Shader* shader) : mesh(new Mesh(vertexBuffer, indexBuffer))
{
	this->renderer = renderer;
	this->particlesShader = shader;
	this->mat = new Material(particlesShader);

	/* Default stuff about every particle */
	this->differenceOnX = 0.2f;
	this->differenceOnY = 0.2f;
	this->differenceOnZ = 0.2f;
	this->particleVelocity = 0.8f;
	this->particleVelocityVariation = 0.2f;
	this->particleSize = 0.03f;
	this->maxParticles = 20;

	this->currentParticleCount = 0;
	this->accumulatedTime = 0.0f;

	this->vertexCount = 0;
	this->indexCount = 0;

	this->vertices = 0;
	this->vertexBuffer = 0;
	this->indexBuffer = 0;

	this->particleList = 0;

	this->worldmatrix = dx::XMMatrixIdentity();
	this->active = true;

	this->fire = false;
}

ParticleSystemComponent::~ParticleSystemComponent()
{
	Shutdown();
}

void ParticleSystemComponent::Shutdown()
{
	if (particlesShader)
	{
		particlesShader = 0;
	}

	if (srv)
	{
		srv->Release();
		srv = 0;
	}

	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}

	if (particleList) {
		delete[] particleList;
		particleList = 0;
	}

	if (vertices) {
		delete vertices;
		vertices = 0;
	}
}

void ParticleSystemComponent::InitializeParticles(ID3D11Device* device, LPCWSTR textureFilename)
{
	//maxParticles = 10;
	particleList = new Particles[maxParticles];

	for (int i = 0; i < maxParticles; i++)
		particleList[i].active = false;


	/* Init and pass it to mesh, can do a initializefunction in mesh later.. or something. */
	InitializeBuffers(device);
	mesh->SetVertexBuffer(vertexBuffer);
	mesh->SetIndexBuffer(indexBuffer);
	mesh->SetIndexCount(indexCount);
	mesh->SetVertexCount(vertexCount);

	LoadTexture(device, textureFilename);
}

void ParticleSystemComponent::InitializeFirelikeParticles(ID3D11Device* device, LPCWSTR textureFilename)
{
	maxParticles = 50;
	particleSize = 0.1f;
	particleList = new Particles[maxParticles];

	for (int i = 0; i < maxParticles; i++)
		particleList[i].active = false;


	particlesPosition.x = GetOwner()->GetTransform().GetPosition().m128_f32[0]-0.05f;
	particlesPosition.y = GetOwner()->GetTransform().GetPosition().m128_f32[1] - 0.02f;
	particlesPosition.z = GetOwner()->GetTransform().GetPosition().m128_f32[2];
	fire = true;

	/* Init and pass it to mesh, can do a initializefunction in mesh later.. or something. */
	InitializeBuffers(device);
	mesh->SetVertexBuffer(vertexBuffer);
	mesh->SetIndexBuffer(indexBuffer);
	mesh->SetIndexCount(indexCount);
	mesh->SetVertexCount(vertexCount);

	LoadTexture(device, textureFilename);
}

void ParticleSystemComponent::SetDifference(float x, float y, float z)
{
	this->differenceOnX = x;
	this->differenceOnY = y;
	this->differenceOnZ = z;
}

void ParticleSystemComponent::LoadTexture(ID3D11Device* device, LPCWSTR textureFilename)
{
	HRESULT hr = dx::CreateWICTextureFromFile(device, textureFilename, nullptr, &srv);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load WIC Texture'", L"Graphics scene Initialization Message", MB_ICONERROR);

	/* Mat info */
	mat->SetTexture(new Texture(srv), TEXTURE_DIFFUSE_SLOT, ShaderBindFlag::PIXEL);
	mat->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device), 0, ShaderBindFlag::PIXEL);
}

void ParticleSystemComponent::InitializeBuffers(ID3D11Device* device)
{
	unsigned int* indices;
	HRESULT result;

	vertexCount = maxParticles * 6;
	indexCount = vertexCount;

	vertices = new Mesh::VertexColor[vertexCount];
	indices = new unsigned int[indexCount];

	for (int i = 0; i < indexCount; i++) {
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Mesh::VertexColor) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	assert(SUCCEEDED(result));


	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	assert(SUCCEEDED(result));

	delete[] indices;
	indices = 0;
}

void ParticleSystemComponent::CreateParticle(float frameTime)
{
	accumulatedTime += frameTime;
	bool emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if (accumulatedTime > 0.1f) {
		accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if ((emitParticle == true) && (currentParticleCount < (maxParticles - 1)))
	{
		currentParticleCount++;

		float positionX = (((float)rand() - (float)rand()) / RAND_MAX) * differenceOnX;
		float positionY = (((float)rand() - (float)rand()) / RAND_MAX) * differenceOnY;
		float positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * differenceOnZ;
		float velocity = particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * particleVelocityVariation;
		float red = 1.0f;
		float green = 1.0f;
		float blue = 1.0f;
		float alpha = 1.0f;

		/*
			Sort the listarray and find a place for the new particle.
			if a particle is not active or has a different depth than the other searched particle-place
			mark that as found and copy the array over by one position from
		*/

		int index = 0;
		bool found = false;
		while (!found)
		{
			if ((particleList[index].active == false) || (particleList[index].posz < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		/*	Sort the list because we have a new currentParticleCount, move everything forwards until we reach the new place	*/
		int i = currentParticleCount;
		int j = i - 1;
		while (i != index)
		{
			particleList[i].posx = particleList[j].posx;
			particleList[i].posy = particleList[j].posy;
			particleList[i].posz = particleList[j].posz;
			particleList[i].red = particleList[j].red;
			particleList[i].green = particleList[j].green;
			particleList[i].blue = particleList[j].blue;
			particleList[i].alpha = particleList[j].alpha;
			particleList[i].velocity = particleList[j].velocity;
			particleList[i].active = particleList[j].active;
			i--;
			j--;
		}

		/* Insert the new particle setup on that index place */
		particleList[index].posx = positionX;
		particleList[index].posy = positionY;
		particleList[index].posz = positionZ;
		particleList[index].red = red;
		particleList[index].green = green;
		particleList[index].blue = blue;
		particleList[index].alpha = alpha;
		particleList[index].velocity = velocity;
		particleList[index].active = true;
	}
}

void ParticleSystemComponent::UpdateParticles(float frameTime)
{
	for (int i = 0; i < currentParticleCount; i++)
	{
		particleList[i].posy = particleList[i].posy + (particleList[i].velocity * ((float)frameTime * 0.5f));

		particleList[i].alpha -= (float)frameTime * 0.5f;
	}
}

void ParticleSystemComponent::DeleteParticles()
{
	/* Kill all the particles that have gone over a certain height range */
	for (int i = 0; i < maxParticles; i++)
	{
		if (/*(particleList[i].active == true) && (particleList[i].posy > 0.5f) ||*/ ((particleList[i].active == true) && particleList[i].alpha <= 0.0f))
		{
			particleList[i].active = false;
			currentParticleCount--;

			/* Shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly */
			for (int j = i; j < maxParticles - 1; j++) {
				particleList[j].posx = particleList[j + 1].posx;
				particleList[j].posy = particleList[j + 1].posy;
				particleList[j].posz = particleList[j + 1].posz;
				particleList[j].red = particleList[j + 1].red;
				particleList[j].green = particleList[j + 1].green;
				particleList[j].blue = particleList[j + 1].blue;
				particleList[j].alpha = particleList[j + 1].alpha;
				particleList[j].velocity = particleList[j + 1].velocity;
				particleList[j].active = particleList[j + 1].active;
			}
		}
	}
}

void ParticleSystemComponent::UpdateBuffers()
{
	/*	Build the vertex array from the particle list array. Each particle is a quad made out of two triangles	*/

	/*
		Resource is mapped for writing, the previous contents of the resource will be undefined.
		Map/Unmap is a common use with temporary buffers. - Dynamic usage of our vertexbuffer. (It is at least prefered)
	*/

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = renderer->GetContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));

	Mesh::VertexColor* verticesPtr = (Mesh::VertexColor*)mappedResource.pData;


	int index = 0;
	for (int i = 0; i < maxParticles; i++)
	{
		// Bottom left.
		verticesPtr[index].position = dx::XMFLOAT3(particleList[i].posx - particleSize, (particleList[i].posy) - particleSize, particleList[i].posz);
		verticesPtr[index].texcoord = dx::XMFLOAT2(0.0f, 1.0f);
		verticesPtr[index].color = dx::XMFLOAT4(particleList[i].red, particleList[i].green, particleList[i].blue, particleList[i].alpha);
		index++;

		// Top left.
		verticesPtr[index].position = dx::XMFLOAT3(particleList[i].posx - particleSize, (particleList[i].posy) + particleSize, particleList[i].posz);
		verticesPtr[index].texcoord = dx::XMFLOAT2(0.0f, 0.0f);
		verticesPtr[index].color = dx::XMFLOAT4(particleList[i].red, particleList[i].green, particleList[i].blue, particleList[i].alpha);
		index++;

		// Bottom right.
		verticesPtr[index].position = dx::XMFLOAT3(particleList[i].posx + particleSize, (particleList[i].posy) - particleSize, particleList[i].posz);
		verticesPtr[index].texcoord = dx::XMFLOAT2(1.0f, 1.0f);
		verticesPtr[index].color = dx::XMFLOAT4(particleList[i].red, particleList[i].green, particleList[i].blue, particleList[i].alpha);
		index++;

		// Bottom right.
		verticesPtr[index].position = dx::XMFLOAT3(particleList[i].posx + particleSize, (particleList[i].posy) - particleSize, particleList[i].posz);
		verticesPtr[index].texcoord = dx::XMFLOAT2(1.0f, 1.0f);
		verticesPtr[index].color = dx::XMFLOAT4(particleList[i].red, particleList[i].green, particleList[i].blue, particleList[i].alpha);
		index++;

		// Top left.
		verticesPtr[index].position = dx::XMFLOAT3(particleList[i].posx - particleSize, (particleList[i].posy) + particleSize, particleList[i].posz);
		verticesPtr[index].texcoord = dx::XMFLOAT2(0.0f, 0.0f);
		verticesPtr[index].color = dx::XMFLOAT4(particleList[i].red, particleList[i].green, particleList[i].blue, particleList[i].alpha);
		index++;

		// Top right.
		verticesPtr[index].position = dx::XMFLOAT3(particleList[i].posx + particleSize, (particleList[i].posy) + particleSize, particleList[i].posz);
		verticesPtr[index].texcoord = dx::XMFLOAT2(1.0f, 0.0f);
		verticesPtr[index].color = dx::XMFLOAT4(particleList[i].red, particleList[i].green, particleList[i].blue, particleList[i].alpha);
		index++;
	}

	//memcpy(verticesPtr, (void*)vertices, (sizeof(Mesh::VertexColor) * index));

	mesh->SetVertexCount(vertexCount);
	renderer->GetContext()->Unmap(vertexBuffer, 0);
}

void ParticleSystemComponent::Update(const float& deltaTime)
{


	/*
		Delete old particles that reaches a height. Create new particles after some time.
		Update the created particles so they fall from a centerposition to a -y position until they disappear.
		Update the dynamic vertexbuffer with the new position of each particle
	*/

	DeleteParticles();
	CreateParticle(deltaTime);
	UpdateParticles(deltaTime);
	UpdateBuffers();
}

void ParticleSystemComponent::Draw(Renderer* renderer, CameraComponent* camera)
{
	if (active)
	{		
		dx::XMMATRIX worldParticles = dx::XMMatrixIdentity();

		if (!fire)
		{
			dx::XMStoreFloat3(&particlesPosition, GetOwner()->GetTransform().GetWorldPosition());
			particlesPosition.y += 0.5f;

			dx::XMFLOAT3 pos;
			dx::XMStoreFloat3(&pos, camera->GetOwner()->GetTransform().GetPosition());

			float rotationPart = atan2(particlesPosition.x - pos.x, particlesPosition.z - pos.z);// *(180.0 / dx::XM_PI);
			//float rotationPart = (float)anglepart * Math::ToRadians;

			dx::XMMATRIX worldParticles = dx::XMMatrixIdentity();
			dx::XMMATRIX particlesRotationY = dx::XMMatrixRotationY(rotationPart);
			dx::XMMATRIX particlesTranslation = dx::XMMatrixTranslation(particlesPosition.x, particlesPosition.y, particlesPosition.z);
			worldParticles = particlesRotationY * particlesTranslation;
			worldmatrix = worldParticles;
		}
		else
		{
			dx::XMMATRIX particlesFireTranslation = dx::XMMatrixTranslation(particlesPosition.x, particlesPosition.y, particlesPosition.z);
			dx::XMMATRIX particlesFireRotationY = dx::XMMatrixRotationY(46);
			worldParticles = particlesFireRotationY * particlesFireTranslation;
			worldmatrix = worldParticles;
		}
		

		renderer->DrawParticles(mesh, mat, worldmatrix);
	}
}
