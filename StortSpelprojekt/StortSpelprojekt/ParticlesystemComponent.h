#pragma once
#include "DXHelper.h"
#include "ResourceManager.h"

class ParticleSystemComponent : public Component
{
private:
	__declspec(align(16))
		struct cb_ObjectPerParticles
	{
		cb_ObjectPerParticles() { ZeroMemory(this, sizeof(this)); }
		DirectX::XMMATRIX worldViewProj;
		DirectX::XMMATRIX world;
	};

	struct Particles {
		float posx, posy, posz;
		float red, green, blue;
		float velocity;
		bool active;
	};

	struct Vertex
	{
		Vertex() : position(), texcoord(), color() {}
		Vertex(float x, float y, float z,
			float u, float v,
			float r, float g, float b, float a)
			: position(x, y, z), texcoord(u, v), color(r, g, b, a) {}

		dx::XMFLOAT3 position;
		dx::XMFLOAT2 texcoord;
		dx::XMFLOAT4 color;
	};

public:
	ParticleSystemComponent(Renderer* renderer, CameraComponent* camera, Shader* shader);
	virtual ~ParticleSystemComponent();
	void Shutdown();

	void InitializeParticles(ID3D11Device* device, LPCWSTR textureFilename);
	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;

	//void Update(float frameTime, CameraComponent* camera, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context, CameraComponent* camera);

	//Object* GetConnectedObject() { return this->object; }

	void SetMaxParticles(int maxParticles) { this->maxParticles = maxParticles; }
	void SetDifference(float x, float y, float z);
	void SetParticleSize(float size) { this->particleSize = size; }

	void SetWorldMatrix(dx::XMMATRIX worldmatrix);
	dx::XMMATRIX GetWorldMatrix() { return worldmatrix; }

	bool GetActive() { return this->active; }
	void SetActive(bool active) { this->active = active; }

private:
	void LoadTexture(ID3D11Device* device, LPCWSTR textureFilename);
	void InitializeBuffers(ID3D11Device* device);

	void SetCBuffers(ID3D11DeviceContext* context, CameraComponent* camera);
	void CreateParticle(float frameTime);
	void UpdateParticles(float frameTime);
	void DeleteParticles();
	void UpdateBuffers();	

private:
	Renderer* renderer;
	Shader* particlesShader;
	//Object* object;
	CameraComponent* camera;

	ID3D11ShaderResourceView* srv;
	ID3D11SamplerState* samplerState;

	/* Particle stuffy stuff */
	float differenceOnX, differenceOnY, differenceOnZ;
	float particleVelocity, particleVelocityVariation;
	float particleSize;
	int maxParticles;
	int currentParticleCount;
	float accumulatedTime;

	/* Buffer stuffy stuff shitty shit */
	int vertexCount, indexCount;
	Vertex* vertices;
	ID3D11Buffer* vertexBuffer, * indexBuffer;
	ID3D11Buffer* bufferPerObjectParticles;
	cb_ObjectPerParticles cbPerObjectParticles;

	/* For dynamicly update */
	Particles* particleList;

	dx::XMMATRIX worldmatrix;
	bool active;
};