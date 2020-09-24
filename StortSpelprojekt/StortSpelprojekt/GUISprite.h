#pragma once
#include <WICTextureLoader.h>
#include "Input.h"
#include <DirectXMath.h>
#include <SpriteBatch.h>
namespace dx = DirectX;
class GUISprite
{
private:

	std::string filePath;
	float xPos, yPos;
	float width, height;
	float xScale, yScale;
	float rotation;
	dx::SpriteBatch* spriteBatch;
	dx::XMVECTOR position, color, origin, scale;
public:

	GUISprite(ID3D11Device* device, std::string, float xPos, float yPos);
	~GUISprite();
	void Draw(dx::SpriteBatch* spriteBatch);
	void SetPosition(float xPos, float yPos);


};