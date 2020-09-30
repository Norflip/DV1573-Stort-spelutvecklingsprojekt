#include "Material.h"

size_t Material::IDCounter = 0;

Material::Material() : ID(IDCounter++) {}

Material::Material(Shader shader) : shader(shader), srv(nullptr), ID(IDCounter++) {}
Material::~Material() {}

void Material::SetSamplerState(ID3D11Device* device, D3D11_TEXTURE_ADDRESS_MODE addressMode, D3D11_FILTER filter)
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = filter;
	samplerDesc.AddressU = addressMode;
	samplerDesc.AddressV = addressMode;
	samplerDesc.AddressW = addressMode;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT samplerResult = device->CreateSamplerState(&samplerDesc, &samplerState);
	assert(SUCCEEDED(samplerResult));

	flag |= ShaderBindFlag::PIXEL;
}

void Material::BindToContext(ID3D11DeviceContext* context) const
{
	this->shader.BindToContext(context);
	BindTextureToContext(context);
}

void Material::SetMaterialData(const cb_Material& materialData)
{
	cb_material_data = materialData;
}

void Material::ChangeTextureBindFlags(size_t slot, ShaderBindFlag flag)
{
	for (int textureNr = 0; textureNr < textures.size(); textureNr++)
	{
		if (textures[textureNr].slot == slot)
		{
			textures[textureNr].flag = flag;
			break;
		}
	}
}

const cb_Material& Material::GetMaterialData() const
{
	return cb_material_data;
}

void Material::BindTextureToContext(ID3D11DeviceContext* context) const
{	
	for(int i = 0; i < textures.size(); i++){
		
		int bflag = static_cast<int>(textures[i].flag);
		auto srv = textures[i].texture.GetTexture();
		size_t slot = textures[i].slot;

		ID3D11ShaderResourceView* const nullsrv[1] = { NULL };
		context->PSSetShaderResources(slot, 1, nullsrv);

		if ((bflag & (int)ShaderBindFlag::PIXEL) != 0) {
			context->PSSetSamplers(slot, 1, &samplerState);
			context->PSSetShaderResources(slot, 1, &srv);
		}
		
		if ((bflag & (int)ShaderBindFlag::VERTEX) != 0) // you can't sample inside a vertex shader
			context->VSSetShaderResources(slot, 1, &srv);

		if ((bflag & (int)ShaderBindFlag::GEOMETRY) != 0)
			context->GSSetShaderResources(slot, 1, &srv);
	}
}

void Material::SetTexture(Texture texture, size_t slot, ShaderBindFlag flag)
{
	TextureInfo newtexture;
	newtexture.texture = texture;
	newtexture.slot = slot;
	newtexture.flag = flag;
	textures.push_back(newtexture);
}