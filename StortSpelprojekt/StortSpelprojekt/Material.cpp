#include "Material.h"

Material::Material() {}

Material::Material(Shader shader) : shader(shader), srv(nullptr){}
Material::~Material() {}

void Material::BindToContext(ID3D11DeviceContext* context)
{
	this->shader.BindToContext(context);
	BindTextureToContext(context);
}

void Material::BindTextureToContext(ID3D11DeviceContext* context)
{	
	for(int i = 0; i < textures.size(); i++){
		
		int bflag = static_cast<int>(textures[i].flag);
		srv = textures[i].texture.GetTexture();
		slot = textures[i].slot;

		ID3D11ShaderResourceView* const nullsrv[1] = { NULL };
		context->PSSetShaderResources(slot, 1, nullsrv);

		if ((bflag & (int)ShaderBindFlag::PIXEL) != 0) 
			context->PSSetShaderResources(slot, 1, &srv);
		
		if ((bflag & (int)ShaderBindFlag::VERTEX) != 0)
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