#pragma once
#include "DXHelper.h"
#include "ResourceManager.h"

class ParticleSystemComponent : public Component
{
private:
	struct Particles {
		float posx, posy, posz;
		float red, green, blue;
		float velocity;
		bool active;
	};	

public:
	ParticleSystemComponent(Renderer* renderer, Shader* shader);
	virtual ~ParticleSystemComponent();
	void Shutdown();

	void InitializeParticles(ID3D11Device* device, const std::string& texureKey);
	void InitializeFirelikeParticles(ID3D11Device* device, const std::string& texureKey);

	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;

	void SetMaxParticles(int maxParticles) { this->maxParticles = maxParticles; }
	void SetDifference(float x, float y, float z);
	void SetParticleSize(float size) { this->particleSize = size; }
		
	bool GetActive() { return this->active; }
	void SetActive(bool active) { this->active = active; }

private:
	void InitializeBuffers(ID3D11Device* device);

	void CreateParticle(float frameTime);
	void UpdateParticles(float frameTime);
	void DeleteParticles();
	void UpdateBuffers();	

private:
	Renderer* renderer;
	Shader* particlesShader;
	ID3D11ShaderResourceView* srv;	

	Bounds cullBounds;

	int vertexCount, indexCount;
	Mesh::VertexColor* vertices;
	ID3D11Buffer* vertexBuffer, * indexBuffer;

	Particles* particleList;
	Mesh* mesh;
	Material* mat;

	dx::XMMATRIX worldmatrix;
	bool active;

	/* Particle stuffy stuff */
	dx::XMFLOAT3 particlesPosition;
	float differenceOnX, differenceOnY, differenceOnZ;
	float particleVelocity, particleVelocityVariation;
	float particleSize;
	int maxParticles;
	int currentParticleCount;
	float accumulatedTime;
	bool fire;
};