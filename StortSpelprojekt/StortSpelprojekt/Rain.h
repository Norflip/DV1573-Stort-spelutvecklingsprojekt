#pragma once
#include "DXHelper.h"
#include "RenderPass.h"
#include "ConstantBuffer.h"
#include "GameClock.h"
#include <random>
namespace dx = DirectX;

class Rain : public RenderPass
{
	const int MAX_PARTICLES = 10000;
	const float MAX_LIFETIME = 10.0f;
	const float Y_OFFSET = 5.0f;

	// INPUT TO CS
	__declspec(align(16))
	struct ParticleInput
	{
		dx::XMFLOAT3 startPosition;
		float speed;
		float timeOffset;
	};

	// OUTPUT FROM COMPUTE SHADER
	__declspec(align(16))
	struct ParticleOutput
	{
		dx::XMFLOAT3 position;
	};

	// CONST BUFFER DATA
	__declspec(align(16))
	struct EmitterData
	{
		dx::XMFLOAT3 emitterPosition;
		float time;
		float lifeTime;
		float pad[3];
	};

public:
	Rain(int priority) : RenderPass(priority, RenderPass::PassType::SKYBOX) {}
	virtual ~Rain() {}

	void m_Initialize(ID3D11Device* device) override;
	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& inTexture, RenderTexture& outTexture) override;

private:
	void InitializeBuffers(ID3D11Device* device);
	void UpdatePositions(dx::XMVECTOR eye, ID3D11DeviceContext* context);
	dx::XMFLOAT3 RandomPointOnDisc(dx::XMFLOAT3 center, float height, float radius);

private:
	ConstantBuffer<EmitterData> emitterDataBuffer;
	ID3D11ComputeShader* computeShader;
	Material material;
	Shader* shader;

	ID3D11Buffer* particleDataBuffer;
	ID3D11Buffer* outputBuffer;
	ID3D11Buffer* vertexBuffer;

	ID3D11ShaderResourceView* inputView;
	ID3D11UnorderedAccessView* outputUAV;

	std::vector<ParticleInput> particleData;
	std::vector<ParticleOutput> particlePositions;

};