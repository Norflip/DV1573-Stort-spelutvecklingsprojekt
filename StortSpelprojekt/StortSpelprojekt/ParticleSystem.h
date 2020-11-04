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
	ParticleSystem(Shader* shader);
	ParticleSystem(const ParticleSystem& other);
	virtual ~ParticleSystem();

	void InitializeParticles(ID3D11Device* device, LPCWSTR textureFilename);
	Object* GetThisObject() { return this->object; }
	Texture GetTexture() { return this->texture; }

	void Shutdown();
	bool Update(float frameTime, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);

	void SetWorldMatrix(dx::XMMATRIX worldmatrix);
	dx::XMMATRIX GetWorldMatrix();

	int GetIndexCount();	

private:
	void LoadTexture(ID3D11Device* device, LPCWSTR textureFilename);
	bool InitializeBuffers(ID3D11Device* device);

	void CreateParticle(float frameTime);
	void UpdateParticles(float frameTime);
	void DeleteParticles();
	bool UpdateBuffers(ID3D11DeviceContext* context);

private:
	HRESULT hr;
	Object* object;
	Material* particlesMaterial;
	Shader* particlesShader;
	Mesh* particleQuad;
	ID3D11ShaderResourceView* srv;
	std::vector<ID3D11ShaderResourceView*> srvs;

	/* Particle stuffy stuff */
	float differenceOnX, differenceOnY, differenceOnZ;
	float particleVelocity, particleVelocityVariation;
	float particleSize;
	int maxParticles;

	int currentParticleCount;
	float accumulatedTime;

	int vertexCount, indexCount;
	Vertex* vertices;
	ID3D11Buffer* vertexBuffer, * indexBuffer;

	Particles* particleList;
	Texture texture;

	dx::XMMATRIX worldmatrix;
};