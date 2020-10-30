#pragma once


#include "DirectXHelpers.h"
#include <WICTextureLoader.h>
#include <SpriteBatch.h>
#include "Renderer.h"
#include <wrl.h>
class GUIManager;

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

ALIGN16
class GUISprite :public GUIObject
{

public:
	GUISprite(Renderer& renderer, std::string, float xPos, float yPos , float layerDepth = 0, DrawDirection dir = DrawDirection::Default, ClickFunction clickFunc = ClickFunction::NotClickable, GuiGroup group = GuiGroup::Default);
	~GUISprite();
	void Draw(DirectX::SpriteBatch*) override;
	void Draw();
	void SetPosition(float xPos, float yPos);
	void SetWICSprite(ID3D11Device* device, std::string spriteFile);
	
	void SetActiveColor(dx::XMVECTOR vector = dx::XMVectorSet(0.6f, 0.6f, 1.3f, 1.0f));

	void SetDDSSprite(ID3D11Device* device, std::string spriteFile); // no dds yet
	void SetScale(float x, float y);
	void SetScaleBars(float yValue);

	bool IsClicked();
	bool IsMouseOver();
	void Update();
		bool HasFlag(ObjectFlag flag) const;
	void AddFlag(ObjectFlag flag);
	void RemoveFlag(ObjectFlag flag);

	ALIGN16_ALLOC;

private:
	std::string filePath;
	float xPos, yPos;
	float relativeXPos, relativeYPos;
	size_t width, height;
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