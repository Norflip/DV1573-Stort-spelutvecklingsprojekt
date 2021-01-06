#include "stdafx.h"
#include "Material.h"


Material::Material() {}
Material::Material(Shader* shader) : shader(shader) {}

Material::~Material() 
{
	textures.clear();
	samplers.clear();
}

void Material::UnbindToContext(ID3D11DeviceContext* context) const
{
	shader->Unbind(context);
}

void Material::BindToContext(ID3D11DeviceContext* context) const
{
	this->shader->BindToContext(context);

	for (auto i : textures)
	{
		for (auto j : i.second)
		{
			size_t slot = j.first;
			auto srv = j.second->GetSRV();

			if ((i.first & (int)ShaderBindFlag::PIXEL) != 0)
				context->PSSetShaderResources(slot, 1, &srv);

			if ((i.first & (int)ShaderBindFlag::VERTEX) != 0)
				context->VSSetShaderResources(slot, 1, &srv);

			if ((i.first & (int)ShaderBindFlag::HULL) != 0)
				context->HSSetShaderResources(slot, 1, &srv);

			if ((i.first & (int)ShaderBindFlag::DOMAINS) != 0)
				context->DSSetShaderResources(slot, 1, &srv);

			if ((i.first & (int)ShaderBindFlag::GEOMETRY) != 0)
				context->GSSetShaderResources(slot, 1, &srv);
			
			if ((i.first & (int)ShaderBindFlag::SOGEOMETRY) != 0)
				context->GSSetShaderResources(slot, 1, &srv);
		}
	}

	for (auto i : samplers)
	{
		for (auto j : i.second)
		{
			size_t slot = j.first;
			auto srv = j.second;

			if ((i.first & (int)ShaderBindFlag::PIXEL) != 0)
				context->PSSetSamplers(slot, 1, &srv);

			if ((i.first & (int)ShaderBindFlag::VERTEX) != 0)
				context->VSSetSamplers(slot, 1, &srv);

			if ((i.first & (int)ShaderBindFlag::HULL) != 0)
				context->HSSetSamplers(slot, 1, &srv);

			if ((i.first & (int)ShaderBindFlag::DOMAINS) != 0)
				context->DSSetSamplers(slot, 1, &srv);

			if ((i.first & (int)ShaderBindFlag::GEOMETRY) != 0)
				context->GSSetSamplers(slot, 1, &srv);

			if ((i.first & (int)ShaderBindFlag::SOGEOMETRY) != 0)
				context->GSSetSamplers(slot, 1, &srv);
		}
	}
}

void Material::SetMaterialData(const cb_Material& materialData)
{
	cb_material_data = materialData;
}

const cb_Material& Material::GetMaterialData() const
{
	return cb_material_data;
}

void Material::SetTexture(Texture* texture, size_t slot, ShaderBindFlag flag)
{
	int flagKey = static_cast<int>(flag);
	if (textures.find(flagKey) == textures.end())
		textures.insert({ flagKey, std::unordered_map<size_t, Texture*>() });
	
	std::unordered_map<size_t, Texture*>& flagMap = textures[flagKey];

	if (flagMap.find(slot) == flagMap.end())
		flagMap.insert({ slot, texture });
	else
		flagMap[slot] = texture;

}

void Material::SetSampler(ID3D11SamplerState* state, size_t slot, ShaderBindFlag flag)
{
	int flagKey = static_cast<int>(flag);
	if (samplers.find(flagKey) == samplers.end())
		samplers.insert({ flagKey, std::unordered_map<size_t, ID3D11SamplerState*>() });

	std::unordered_map<size_t, ID3D11SamplerState*>& flagMap = samplers[flagKey];
	if (flagMap.find(slot) == flagMap.end())
		flagMap.insert({ slot, state });
	else
		flagMap[slot] = state;
}
