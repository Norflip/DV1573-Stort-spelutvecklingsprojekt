#pragma once
#include "Shader.h"
#include "Texture.h"
#include <vector>


class Material
{
	struct TextureInfo {
		Texture texture;
		size_t slot;
		ShaderBindFlag flag;
	};

public:
	Material();
	Material (Shader shader);
	virtual ~Material();
	
	void SetShader(Shader shader) { this->shader = shader; }

	/* Set /Create sampler state */
	void SetSamplerState(ID3D11Device* device, D3D11_TEXTURE_ADDRESS_MODE addressMode, D3D11_FILTER filter);

	void BindToContext(ID3D11DeviceContext*);
	void BindTextureToContext(ID3D11DeviceContext*);

	/* Binding texture to correct slot in shader based on slot-input */
	void SetTexture(Texture texture, size_t slot, ShaderBindFlag flag);
	Texture GetTexture() { return this->texture; }

private:
	Shader shader;
	ID3D11ShaderResourceView* srv;
	Texture texture;
	size_t slot;
	ShaderBindFlag flag;

	ID3D11SamplerState* samplerState;

	std::vector<TextureInfo> textures;
};