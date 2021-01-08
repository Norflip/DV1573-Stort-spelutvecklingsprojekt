#pragma once
#include "DirectXHelpers.h"
#include "CameraComponent.h"

const dx::XMFLOAT4 fireRedColor = dx::XMFLOAT4(0.5f, 0.2f, 0.2f, 1.0f);
const dx::XMFLOAT4 grayColor = dx::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);


class Particlesys
{

private:	

	__declspec(align(16))
		struct cBufferPerFrame
	{
		cBufferPerFrame() { ZeroMemory(this, sizeof(this)); }

		dx::XMFLOAT4 particleColor;
		dx::XMFLOAT3 eyePos;
		float padding;

		dx::XMFLOAT3 emitPos;
		float padding2;

		dx::XMFLOAT3 emitDir;
		float gameTime;

		float ageTimeStep;

		float particleMaxAge;
		float padding3[2];

		dx::XMMATRIX viewProjection;

		int usingTexture;
		dx::XMFLOAT3 particleSpreadMulti;

		int particlesPerSecond;
		dx::XMFLOAT2 particleSize;
		int active;
	};

public:
	/* particle shit for input layout */
	struct Particle
	{
		dx::XMFLOAT3 startPos;
		dx::XMFLOAT3 startVelocity;
		dx::XMFLOAT2 size;
		float age;
		unsigned int type;
	};

public:
	Particlesys(Shader* soShader, Shader* drawShader);
	~Particlesys();

	void InitializeParticles(ID3D11Device* device, Renderer* renderer,Object* objectRef);

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
	void SetParticlesActive(bool active) { this->particlesActivated = active; }

	dx::XMFLOAT3 GetEyePos() { return this->eyePos; }
	dx::XMFLOAT3 GetEmitPos() { return this->emitPos; }
	dx::XMFLOAT3 GetEmitDir() { return this->emitDir; }
	dx::XMFLOAT4 GetParticleColor() { return this->particleColor; }
	int GetMaxParticles() { return this->maxParticles; }
	dx::XMFLOAT3 GetParticleSpreadMulti() { return this->particleSpreadMulti; }
	float GetMaxParticleAge() { return this->particleMaxAge; }
	dx::XMFLOAT2 GetParticleSize() { return this->particleSize; }
	int GetParticlesPerSecond() { return this->particlesPerSecond; }

	float GetGameTime() { return this->gameTimer; }
	float GetTimeStep() { return this->ageTimeStep; }

	ID3D11ShaderResourceView* GetParticleTexture() { return this->particleSRV; }
	ID3D11ShaderResourceView* GetRandomTexture() { return this->randomNumberSRV; }
	void SetTexture(ID3D11Device* device, LPCWSTR particleTexture);

	void Reset();
	void Update(float deltaTime, float gameTime, float fuel);
	void Draw(ID3D11DeviceContext* context, CameraComponent* cam);

	float RandomFloat(float a, float b);
	
private:
	void DrawStreamOut(ID3D11DeviceContext* context, CameraComponent* cam);
	void DrawParticles(ID3D11DeviceContext* context, CameraComponent* cam);	
	void BuildVB(ID3D11Device* device);

private:
	Renderer* renderer;
	Object* objectRef;

	Shader* drawShader;
	Shader* streamoutShader;

	int maxParticles;
	bool firstRun;

	float gameTimer;
	float ageTimeStep;
	float particleAge;
	float particleMaxAge;
	bool usingTexture;

	dx::XMFLOAT4 particleColor;
	dx::XMFLOAT4 particleColorModify;
	dx::XMFLOAT3 eyePos;
	dx::XMFLOAT3 emitPos;
	dx::XMFLOAT3 emitDir;
	dx::XMFLOAT2 particleSize;
	dx::XMFLOAT2 particleSizeModify;
	dx::XMFLOAT3 particleSpreadMulti;
	dx::XMFLOAT3 particleSpreadModify;

	bool particlesActivated;

	ID3D11Buffer* initializeVB;
	ID3D11Buffer* drawVB;
	ID3D11Buffer* streamoutVB;

	ID3D11ShaderResourceView* particleSRV;
	ID3D11ShaderResourceView* randomNumberSRV;

	HRESULT hr;	

	ID3D11SamplerState* sampler;	
	cBufferPerFrame cbPerFrame;
	ID3D11Buffer* cbufferPerFrame;
	ID3D11Device* device;	

	/* Benchmark stuff */
	int particlesPerSecond;	

	bool left = false, right = true;
};