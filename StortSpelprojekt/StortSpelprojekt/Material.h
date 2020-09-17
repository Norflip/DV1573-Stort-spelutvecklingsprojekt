#pragma once
#include "Shader.h"
#include "Texture.h"

class Material
{
public:
	Material (Shader shader);
	virtual ~Material();
	
	void BindToContext(ID3D11DeviceContext*);

	/* Binding texture to correct slot in shader based on slot-input */
	void SetTexture(ID3D11DeviceContext* context, Texture texture, size_t slot, ShaderBindFlag flag);

private:
	Shader shader;
	ID3D11ShaderResourceView* srv;
	ID3D11SamplerState* sampler;
};
