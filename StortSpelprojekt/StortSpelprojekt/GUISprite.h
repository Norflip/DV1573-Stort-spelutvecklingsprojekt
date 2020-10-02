#pragma once

#include "GUIManager.h"
#include "DirectXHelpers.h"
#include <WICTextureLoader.h>
#include <DirectXMath.h>
#include <SpriteBatch.h>
#include "Renderer.h"
#include <wrl.h>
#include <iostream>
namespace dx = DirectX;

enum DrawDirection : uint32_t
{
	Default = 0,
	TopLeft = 0,
	TopRight = 1,
	BottomLeft = 2,
	BottomRight = TopRight | BottomLeft,
};


class GUISprite :public GUIObject
{

public:

	GUISprite(Renderer& renderer, std::string, float xPos, float yPos , DrawDirection dir);
	~GUISprite();
	void Draw(DirectX::SpriteBatch*) override;
	void Draw();
	void SetPosition(float xPos, float yPos);
	void SetWICSprite(ID3D11Device* device, std::string spriteFile);
	void SetActive(bool set = true) { this->active = set; };
	void SetActiveColor();
	bool GetActive() { return this->active; };
	void SetDDSSprite(ID3D11Device* device, std::string spriteFile); // no dds yet
private:
	bool active = true;
	std::string filePath;
	float xPos, yPos;
	float width, height;
	float xScale, yScale;
	float rotation;
	std::unique_ptr<dx::SpriteBatch> spriteBatch;
	Renderer* renderer;
	dx::XMVECTOR position, color, origin, scale; //byt till xmfloat 4
	ID3D11ShaderResourceView* SRV;
	DrawDirection direction;
	void setPos(float xPos, float yPos, DrawDirection dir);

};