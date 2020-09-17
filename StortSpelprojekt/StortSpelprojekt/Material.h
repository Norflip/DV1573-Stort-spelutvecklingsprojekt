#pragma once
#include "Shader.h"
#include "Buffers.h"

class Material
{
public:
	Material (Shader shader);
	virtual ~Material();
	
	void BindToContext(ID3D11DeviceContext*);
	//void SetTexture(size_t slot);
	//void SetSampler(size_t slot);
	void SetMaterialData(const cb_Material& materialData);
	const cb_Material& GetMaterialData() const;

private:
	Shader shader;
	cb_Material cb_material_data;
};
