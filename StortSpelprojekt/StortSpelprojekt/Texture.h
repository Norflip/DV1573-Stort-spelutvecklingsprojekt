#pragma once
#include "..\packages\directxtk_desktop_win10.2020.8.15.1\include\WICTextureLoader.h"

#define TEXTURE_DIFFUSE_SLOT 0
#define TEXTURE_NORMAL_SLOT 1

#define BONES_SRV_SLOT 2

#define GRASS_STRAWS_SRV_SLOT 3
#define GRASS_INDICES_SRV_SLOT 4
#define GRASS_COORD_SRV_SLOT 5

#define TEXTURE_HEIGHT_SLOT 1
#define TEXTURE_NOISE_SLOT 2



#define TEXTURE_DIFFUSE2_SLOT 2
#define TEXTURE_DIFFUSE3_SLOT 3
#define TEXTURE_DIFFUSE4_SLOT 4
#define TEXTURE_DIFFUSE5_SLOT 5

#define TEXTURE_ALPHA_SLOT 3
#define TEXTURE_CUBE_SLOT 2


ALIGN16
class Texture : public Resource
{
public:
	Texture();
	Texture(ID3D11ShaderResourceView* srv);
	Texture(ID3D11ShaderResourceView* srv, size_t width, size_t height, size_t channels);
	virtual ~Texture();

	Texture* CreateRandom1DTexture(ID3D11Device* device);
	static Texture* CreateFromBuffer (unsigned char* buffer, size_t width, size_t height, size_t channels, DXGI_FORMAT format, ID3D11Device* device);
	static Texture* LoadTexture (ID3D11Device* device, LPCWSTR textureFilepath);

	void SetSRV(ID3D11ShaderResourceView* srv) { this->srv = srv; }
	ID3D11ShaderResourceView* GetSRV() const { return this->srv; }
	
	ALIGN16_ALLOC;
private:
	float RandomFloat(float a, float b);

private:
	ID3D11ShaderResourceView* srv;	
	size_t width, height, channels;
};

static std::unordered_map<std::string, Texture*> mTextureCache;