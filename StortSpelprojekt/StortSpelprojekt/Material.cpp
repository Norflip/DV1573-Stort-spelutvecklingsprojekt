#include "Material.h"

Material::Material(Shader shader) : shader(shader), srv(nullptr), sampler(nullptr) {}
Material::~Material() {}

void Material::BindToContext(ID3D11DeviceContext* context)
{
	this->shader.BindToContext(context);
}

void Material::SetTexture(ID3D11DeviceContext* context, Texture texture, size_t slot, ShaderBindFlag flag)
{
	int bflag = static_cast<int>(flag);
	srv = texture.GetTexture();
	sampler = texture.GetSampler();

	if ((bflag & (int)ShaderBindFlag::PIXEL) != 0) {
		context->PSSetShaderResources(slot, 1, &srv);
	}		

	if ((bflag & (int)ShaderBindFlag::VERTEX) != 0)
		context->VSSetShaderResources(slot, 1, &srv);

	if ((bflag & (int)ShaderBindFlag::GEOMETRY) != 0)
		context->GSSetShaderResources(slot, 1, &srv);
}
