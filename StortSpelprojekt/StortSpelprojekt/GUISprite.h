#pragma once
#include "DirectXHelpers.h"
#include <WICTextureLoader.h>
#include <DirectXMath.h>
#include <SpriteBatch.h>
#include "Renderer.h"
#include <wrl.h>
#include <iostream>
namespace dx = DirectX;
class GUISprite
{

public:

	GUISprite(Renderer& renderer, std::string, float xPos, float yPos);
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
	std::unique_ptr<dx::SpriteBatch> spriteBatch;
	Renderer* renderer;
	dx::XMVECTOR position, color, origin, scale;
	ID3D11ShaderResourceView* SRV;
	

};