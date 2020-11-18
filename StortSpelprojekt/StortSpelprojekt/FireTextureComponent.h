#pragma once
#include "DXHelper.h"
#include "ResourceManager.h"

class FireTextureComponent : public Component
{

public:
	FireTextureComponent(Renderer* renderer, Shader* shader);
	virtual ~FireTextureComponent();
	void Initialize(ID3D11Device* device);
	void Shutdown();

	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;	

private:
	void LoadTextures(ID3D11Device* device);
	void InitializeBuffers(ID3D11Device* device);	

private:
	Mesh* mesh;
	Material* meshMaterial;

	Renderer* renderer;
	Shader* particlesShader;
	ID3D11SamplerState* sampleState_LinearWrap;
	ID3D11SamplerState* sampleState_LinearClamp;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
	Shader* fireShader;

	ID3D11ShaderResourceView* fireTexture;		
	ID3D11ShaderResourceView* noiseTexture;
	ID3D11ShaderResourceView* alphaTexture;

	ID3D11Buffer* objectBuffer;
	ID3D11Buffer* noiseBuffer;
	ID3D11Buffer* disortionBuffer;
};
