#include "Material.h"

Material::Material(Shader shader) : shader(shader) {}
Material::~Material() {}

void Material::BindToContext(ID3D11DeviceContext* context)
{
	this->shader.BindToContext(context);
	
}

void Material::SetMaterialData(const cb_Material& materialData)
{
	cb_material_data = materialData;
}

const cb_Material& Material::GetMaterialData() const
{
	return cb_material_data;
}
