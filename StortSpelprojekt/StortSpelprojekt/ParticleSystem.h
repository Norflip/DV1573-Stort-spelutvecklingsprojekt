#pragma once
#include "DXHelper.h"

class ParticleSystem
{
private:
	struct Particles {
		float posx, posy, posz;
		float red, green, blue;
		float velocity;
		bool active;
	};

public:
	ParticleSystem();
	ParticleSystem(const ParticleSystem& other);
	virtual ~ParticleSystem();

	bool Initialize(ID3D11Device* device, LPCWSTR textureFilename);
	void Shutdown();
	bool Update(float frameTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);

	void SetWorldMatrix(dx::XMMATRIX worldmatrix);
	dx::XMMATRIX GetWorldMatrix();
	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

private:
	bool LoadTexture(ID3D11Device* device, LPCWSTR textureFilename);
	bool InitializeBuffers(ID3D11Device* device);

	void CreateParticle(float frameTime);
	void UpdateParticles(float frameTime);
	void DeleteParticles();
	bool UpdateBuffers(ID3D11DeviceContext* context);

private:
	float differenceOnX, differenceOnY, differenceOnZ;
	float particleVelocity, particleVelocityVariation;
	float particleSize;
	int maxParticles;

	int currentParticleCount;
	float accumulatedTime;

	

	Particles* particleList;
	Texture* texture;

	dx::XMMATRIX worldmatrix;
};