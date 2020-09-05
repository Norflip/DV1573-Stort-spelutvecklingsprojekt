#pragma once
#include "Shader.h"

class Material
{
public:
	Material (Shader shader);
	virtual ~Material();
	
	void BindToContext(ID3D11DeviceContext*);
	
private:
	Shader shader;
};

//using DefaultMaterial = Material<TestMaterialConstantBuffer>