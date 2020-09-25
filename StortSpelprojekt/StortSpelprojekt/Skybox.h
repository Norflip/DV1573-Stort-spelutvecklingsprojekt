#pragma once

#include "Shader.h"
#include "ZWEBLoader.h"

#include "../include/DDSTextureLoader.h"

class Skybox
{
public:
	Skybox(ID3D11Device* device, Object* object);
	~Skybox();

	void LoadDDS(ID3D11DeviceContext* context, ID3D11Device* device, LPCWSTR textureFilename);
	Object* GetObjectMesh() { return this->object; }
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
};