#pragma once
#include "ZWEBLoader.h"
#include "Shader.h"

class Skybox
{
public:
	Skybox(Object* object);
	~Skybox();

	void LoadDDS();

	void SetDiffuse(dx::XMFLOAT4);
	void SetAmbient(dx::XMFLOAT4);
	void SetSpecular(dx::XMFLOAT4);

	dx::XMFLOAT4 GetDiffuse() { return this->diffuse; }
	dx::XMFLOAT4 GetAmbient() { return this->ambient; }
	dx::XMFLOAT4 GetSpecular() { return this->specular; }

private:	
	Object* skybox;

	dx::XMFLOAT4 diffuse;
	dx::XMFLOAT4 ambient;
	dx::XMFLOAT4 specular;
};