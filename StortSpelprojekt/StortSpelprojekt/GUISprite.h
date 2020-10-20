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

enum ClickFunction
{
	Clickable = 0,
	NotClickable = 1,
};

class GUISprite :public GUIObject
{

public:
	GUISprite(Renderer& renderer, std::string, float xPos, float yPos , float layerDepth = 0, DrawDirection dir = DrawDirection::Default, ClickFunction clickFunc = ClickFunction::NotClickable);
	~GUISprite();
	void Draw(DirectX::SpriteBatch*) override;
	void Draw();
	void SetPosition(float xPos, float yPos);
	void SetWICSprite(ID3D11Device* device, std::string spriteFile);
	void SetActive(bool set = true) { this->active = set; };
	void SetActiveColor(dx::XMVECTOR vector = dx::XMVectorSet(0.6f, 0.6f, 1.3f, 1.0f));
	bool GetActive() { return this->active; };
	void SetDDSSprite(ID3D11Device* device, std::string spriteFile); // no dds yet
	
	bool IsClicked();
	bool IsMouseOver();
	void Update();

private:
	bool active = false;
	std::string filePath;
	float xPos, yPos;
	float relativeXPos, relativeYPos;
	float width, height;
	float xScale, yScale;
	float rotation;
	float layerDepth;
	std::unique_ptr<dx::SpriteBatch> spriteBatch;
	Renderer* renderer;
	dx::XMVECTOR position, baseColor,activeColor, origin, scale; //byt till xmfloat 4
	ID3D11ShaderResourceView* SRV;
	DrawDirection direction;
	void SetPos(float xPos, float yPos, DrawDirection dir);

	ClickFunction clickFunc;
};