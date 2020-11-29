#pragma once
#include "DXHelper.h"
#include "ResourceManager.h"

class ParticleComponent : public Component
{
public:
	ParticleComponent(Renderer* renderer, Shader* shader);
	virtual ~ParticleComponent();
	
	void InitializeParticles(ID3D11Device* device);

	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;
	
	// Time elapsed since the system was reset.
	float GetAge() { return this->particleAge; };

	void SetEyePos(dx::XMFLOAT3 eyePosW) { this->eyePos = eyePosW; }
	void SetEmitPos(dx::XMFLOAT3 emitPosW) { this->emitPos = emitPosW; }
	void SetEmitDir(dx::XMFLOAT3 emitDirW) { this->emitDir = emitDirW; }
	void SetParticleColor(dx::XMFLOAT4 particleColor) { this->particleColor = particleColor; }
	void SetMaxParticles(int particles) { maxParticles = particles; }
	void SetParticleSpreadMulti(dx::XMFLOAT3 particleSpread) { particleSpreadMulti = particleSpread; }
	void SetMaxParticleAge(float particlesMaxAge) { particleMaxAge = particlesMaxAge; }
	void SetParticleSize(dx::XMFLOAT2 particleSize) { this->particleSize = particleSize; }
	void SetParticlesPerSecond(int particlesPerSec) { this->particlesPerSecond = particlesPerSec; }

	dx::XMFLOAT3 GetEyePos() { return this->eyePos; }
	dx::XMFLOAT3 GetEmitPos() { return this->emitPos; }
	dx::XMFLOAT3 GetEmitDir() { return this->emitDir; }
	dx::XMFLOAT4 GetParticleColor() { return this->particleColor; }
	int GetMaxParticles() { return this->maxParticles; }
	dx::XMFLOAT3 GetParticleSpreadMulti() { return this->particleSpreadMulti; }
	float GetMaxParticleAge() { return this->particleMaxAge; }
	dx::XMFLOAT2 GetParticleSize() { return this->particleSize; }
	int GetParticlesPerSecond() { return this->particlesPerSecond; }

	ID3D11ShaderResourceView* GetParticleTexture() { return this->particleSRV; }
	ID3D11ShaderResourceView* GetRandomTexture() { return this->randomNumberSRV; }
	void SetTexture(ID3D11Device* device, LPCWSTR particleTexture);

	void Reset();

	bool GetActive() { return this->active; }
	void SetActive(bool active) { this->active = active; }
	float RandomFloat(float a, float b);

private:
	void BuildVertexBuffers(ID3D11Device* device);
	void DrawStreamOut(ID3D11DeviceContext* context, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, CameraComponent* cam, ID3D11SamplerState* sampler);
	void Draw(ID3D11DeviceContext* context, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, CameraComponent* cam, ID3D11SamplerState* sampler);

private:
	bool active;
	int particlesPerSecond;

	int maxParticles;
	bool firstRun;

	float gameTimer;
	float ageTimeStep;
	float particleAge;
	float particleMaxAge;
	bool usingTexture;

	dx::XMFLOAT4 particleColor;
	dx::XMFLOAT3 eyePos;
	dx::XMFLOAT3 emitPos;
	dx::XMFLOAT3 emitDir;
	dx::XMFLOAT2 particleSize;
	dx::XMFLOAT3 particleSpreadMulti;

	ID3D11Buffer* initializeVB;
	ID3D11Buffer* drawVB;
	ID3D11Buffer* streamoutVB;

	ID3D11ShaderResourceView* particleSRV;
	ID3D11ShaderResourceView* randomNumberSRV;

	cb_particle particleBuffer; 
	ID3D11Buffer* cb_Per_Particle;

	HRESULT hr;
};
