#include "stdafx.h"
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	this->differenceOnX = 0.0f;
	this->differenceOnY = 0.0f;
	this->differenceOnZ = 0.0f;
	this->particleVelocity = 0.0f;
	this->particleVelocityVariation = 0.0f;
	this->particleSize = 0.0f;
	this->maxParticles = 0;

	this->currentParticleCount = 0;
	this->accumulatedTime = 0.0f;

	

	this->particleList = 0;
	this->texture = 0;

	this->worldmatrix = dx::XMMatrixIdentity();
}

ParticleSystem::ParticleSystem(const ParticleSystem& other)
{
	this->differenceOnX = other.differenceOnX;
	this->differenceOnY = other.differenceOnY;
	this->differenceOnZ = other.differenceOnZ;
	this->particleVelocity = other.particleVelocity;
	this->particleVelocityVariation = other.particleVelocityVariation;
	this->particleSize = other.particleSize;
	this->maxParticles = other.maxParticles;

	this->currentParticleCount = other.currentParticleCount;
	this->accumulatedTime = other.accumulatedTime;

	

	this->particleList = other.particleList;
	this->texture = other.texture;

	this->worldmatrix = other.worldmatrix;
}

ParticleSystem::~ParticleSystem()
{
	Shutdown();
}

bool ParticleSystem::Initialize(ID3D11Device* device, LPCWSTR textureFilename)
{
	bool result;

	/*
		Deviation on each axis. (A random deviation). Is from the point on the scene each particle will be spawned and (random -2 to 2 in x-axis and z-axis).
		Speed (Velocity) for each particle. + Variation makes it between 9.8f - 10.2f
		Particlesize is based on the created quad.
	*/

	differenceOnX = 2.0f;
	differenceOnY = 0.2f;
	differenceOnZ = 2.0f;

	particleVelocity = 10.0f;
	particleVelocityVariation = 0.2f;

	particleSize = 0.2f;


	/*
		Set a maximum amount of particles allowed at the same time in the scene.
		Create the list of particles.
		Init all of them to false (not active on the scene.)
	*/

	maxParticles = 50;
	particleList = new Particles[maxParticles];
	if (!particleList) {
		return false;
	}

	for (int i = 0; i < maxParticles; i++) {
		particleList[i].active = false;
	}


	/*
		Currentparticlecount is the current particles falling down in the scene at the moment. (0 at the beginning)
		Accumulated time is the time that increments based on deltatime. When a certain time is reached, a new particle is created.
	*/
	currentParticleCount = 0;
	accumulatedTime = 0.0f;

	
	result = LoadTexture(device, textureFilename);
	if (!result)
		return false;

	return true;
}

void ParticleSystem::Shutdown()
{

}

bool ParticleSystem::Update(float frameTime, ID3D11DeviceContext* context)
{
	bool result;

	/*
		Delete old particles that reaches a height.
		Create new particles after some time.
		Update the created particles so they fall from a centerposition to a -y position until they disappear.

		Update the dynamic vertexbuffer with the new position of each particle
	*/

	DeleteParticles();
	CreateParticle(frameTime);
	UpdateParticles(frameTime);

	/* Update the dynamic vertex buffer with the new position of each particle. */
	result = UpdateBuffers(context);
	if (!result)
		return false;

	return true;
}

void ParticleSystem::Render(ID3D11DeviceContext* context)
{
}

void ParticleSystem::SetWorldMatrix(dx::XMMATRIX worldmatrix)
{
}

dx::XMMATRIX ParticleSystem::GetWorldMatrix()
{
	return dx::XMMATRIX();
}

ID3D11ShaderResourceView* ParticleSystem::GetTexture()
{
	return nullptr;
}

int ParticleSystem::GetIndexCount()
{
	return 0;
}

bool ParticleSystem::LoadTexture(ID3D11Device* device, LPCWSTR textureFilename)
{
	return false;
}

bool ParticleSystem::InitializeBuffers(ID3D11Device* device)
{
	return false;
}

void ParticleSystem::CreateParticle(float frameTime)
{
}

void ParticleSystem::UpdateParticles(float frameTime)
{
}

void ParticleSystem::DeleteParticles()
{
}

bool ParticleSystem::UpdateBuffers(ID3D11DeviceContext* context)
{
	return false;
}
