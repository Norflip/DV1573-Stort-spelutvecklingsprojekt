#pragma once
#include "Shader.h"

class Material
{
public:
	Material (Shader shader);
	virtual ~Material();
	
	void BindToContext(ID3D11DeviceContext*);
	//void SetTexture(size_t slot);
	//void SetSampler(size_t slot);

private:
	Shader shader;
};
