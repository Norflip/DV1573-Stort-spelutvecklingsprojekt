#pragma once
#include "DXHelper.h"
#include "ResourceManager.h"

class FireTextureComponent : public Component
{

public:
	FireTextureComponent(Renderer* renderer, Shader* shader);
	virtual ~FireTextureComponent();
	
	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;	

private:
	void LoadTexture(ID3D11Device* device, LPCWSTR textureFilename);
	void InitializeBuffers(ID3D11Device* device);	

private:
	Renderer* renderer;
	Shader* particlesShader;
	ID3D11ShaderResourceView* srv;		
};
