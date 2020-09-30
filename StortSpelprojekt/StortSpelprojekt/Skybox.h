#pragma once
#include <vector>
#include "Shader.h"
#include "ZWEBLoader.h"

#include "../include/DDSTextureLoader.h"

class Skybox
{
public:
	Skybox(ID3D11Device* device, ID3D11DeviceContext* context, Object* object);
	~Skybox();

	void LoadAllTextures(ID3D11DeviceContext* context, ID3D11Device* device);
	Object* GetThisObject() { return this->object; }
	Texture GetTexture() { return this->texture; }

	void SetDiffuse(dx::XMFLOAT4);
	void SetAmbient(dx::XMFLOAT4);
	void SetSpecular(dx::XMFLOAT4);

	dx::XMFLOAT4 GetDiffuse() { return this->diffuse; }
	dx::XMFLOAT4 GetAmbient() { return this->ambient; }
	dx::XMFLOAT4 GetSpecular() { return this->specular; }

private:	
	HRESULT hr;
	Shader skyboxShader;
	Object* object;

	Mesh skyboxMesh;
	Material skyboxMaterial;

	Texture texture;

	dx::XMFLOAT4 diffuse;
	dx::XMFLOAT4 ambient;
	dx::XMFLOAT4 specular;

	ID3D11ShaderResourceView* srv;
	std::vector<ID3D11ShaderResourceView*> srvs;
};