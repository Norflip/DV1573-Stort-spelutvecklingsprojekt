#pragma once
#include "DXHelper.h"
#include "ResourceManager.h"
#include "Buffers.h"

class FireTextureComponent : public Component
{
private:
	
	ALIGN16
		struct cb_Noise
	{
		float frameTime;
		dx::XMFLOAT3 scrollSpeeds;
		dx::XMFLOAT3 scales;
		float padding;
	};

	ALIGN16
		struct cb_Disortion
	{
		dx::XMFLOAT2 distortion1;
		dx::XMFLOAT2 distortion2;
		dx::XMFLOAT2 distortion3;
		float distortionScale;
		float distortionBias;
	};

public:
	FireTextureComponent(Renderer* renderer, Shader* shader);
	virtual ~FireTextureComponent();
	void InitializeFire(ID3D11Device* device);
	void Shutdown();
	Object* GetFireObject() { return this->fireObject; }
	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;	

private:
	void LoadTextures(ID3D11Device* device);
	void InitializeBuffers(ID3D11Device* device);	
	void UpdateBuffers();

private:
	//Mesh* mesh;
	//Material* meshMaterial;

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
	dx::XMMATRIX quadMatrix;
	std::vector<Material*> quadMaterial;
	std::vector<Mesh*> quadMesh;

	Object* fireObject;

	cb_Noise noise;
	cb_Disortion disortion;
};
