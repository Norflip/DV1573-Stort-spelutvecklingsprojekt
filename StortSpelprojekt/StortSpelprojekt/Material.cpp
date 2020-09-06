#include "Material.h"

Material::Material(Shader shader) : shader(shader) {}
Material::~Material() {}

void Material::BindToContext(ID3D11DeviceContext* context)
{
	this->shader.BindToContext(context);
	this->shader.BindSkeletonToContext(context);
}
