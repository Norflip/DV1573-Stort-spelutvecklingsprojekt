#pragma once
#include "Shader.h"
#include "Buffers.h"
#include "Texture.h"
#include <vector>


class Material
{
	static size_t IDCounter;

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
	//void SetTexture(size_t slot);
	//void SetSampler(size_t slot);
	void SetMaterialData(const cb_Material& materialData);
	const cb_Material& GetMaterialData() const;


	void BindTextureToContext(ID3D11DeviceContext*);

	/* Binding texture to correct slot in shader based on slot-input */
	void SetTexture(Texture texture, size_t slot, ShaderBindFlag flag);
	Texture GetTexture() { return this->texture; }

	//ZWEB DEFAULTS TEXTURES AS INPUTS TO PIXEL SO THIS CAN BE USED TO MANUALLY CHANGE BIND FLAGS
	void ChangeTextureBindFlags(size_t slot, ShaderBindFlag flag);

	void SetName(const std::string& name) { this->name = name; }
	const std::string& GetName() const { return this->name; }

	size_t GetID() const { return this->ID; }

private:
	Shader shader;
	cb_Material cb_material_data;
	size_t ID;

	ID3D11ShaderResourceView* srv;
	Texture texture;
	size_t slot;
	ShaderBindFlag flag;

	ID3D11SamplerState* samplerState;
	std::string name;
	std::vector<TextureInfo> textures;
	
};
