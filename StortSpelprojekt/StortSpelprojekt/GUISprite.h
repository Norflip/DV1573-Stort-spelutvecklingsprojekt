#pragma once
#include "DirectXHelpers.h"
#include <WICTextureLoader.h>
#include "Input.h"
#include <DirectXMath.h>
#include <SpriteBatch.h>
#include "DDSTextureLoader.h"
namespace dx = DirectX;
class GUISprite
{

public:

	GUISprite(ID3D11Device* device, std::string, float xPos, float yPos);
	~GUISprite();
	void Draw();
	void SetPosition(float xPos, float yPos);
	void SetWICSprite(ID3D11Device* device, std::string spriteFile);
	void SetDDSSprite(ID3D11Device* device, std::string spriteFile); // no dds yet
private:

	std::string filePath;
	float xPos, yPos;
	float width, height;
	float xScale, yScale;
	float rotation;
	dx::SpriteBatch* spriteBatch;
	ID3D11Device* device;
	dx::XMVECTOR position, color, origin, scale;
	ID3D11ShaderResourceView* SRV;
	

};