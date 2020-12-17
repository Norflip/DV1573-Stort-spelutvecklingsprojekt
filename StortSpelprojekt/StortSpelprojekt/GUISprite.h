#pragma once

#include <wincodec.h>
#include "DirectXHelpers.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <SpriteBatch.h>
#include "Renderer.h"
#include <wrl.h>
#include <ScreenGrab.h>
class GUIManager;

enum DrawDirection : uint32_t
{


	Default = 0,
	TopLeft = 1 << 0,
	TopRight = 1 << 1,
	BottomLeft = 1 << 2,
	Center = 1 <<3,
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
	GUISprite();
	GUISprite(Renderer& renderer, std::string path, float xPos, float yPos , float layerDepth = 0, DrawDirection dir = DrawDirection::Default, ClickFunction clickFunc = ClickFunction::NotClickable, GuiGroup group = GuiGroup::Default);
	virtual ~GUISprite();
	virtual void Draw(DirectX::SpriteBatch*) override;
	virtual void Draw();
	void SetPosition(float xPos, float yPos);

	float GetWidth() { return FCAST(width); };
	float GetHeight() { return FCAST(width); };

	float GetXpos() { return xPos; };
	float GetYpos() { return yPos; };
	
	void SetWICSprite(ID3D11Device* device, std::string spriteFile);
	void SetColor(sm::Vector4 color);
	sm::Vector4 GetColor() { return baseColor; };
	void SetActiveColor(dx::XMVECTOR vector = dx::XMVectorSet(0.6f, 0.6f, 1.3f, 1.0f));
	void SetScaleColor(float value);

	void SetDDSSprite(ID3D11Device* device, std::string spriteFile); // no dds yet
	void SetScale(float x, float y);
	void SetScaleBars(float yValue);
	void Move(POINTFLOAT moveDir);
	bool IsClicked();
	bool IsMouseOver();
	virtual void Update() override;
	bool HasFlag(ObjectFlag flag) const;
	void AddFlag(ObjectFlag flag);
	void RemoveFlag(ObjectFlag flag);

	ALIGN16_ALLOC;

protected:
	std::string filePath;
	float xPos, yPos;
	// help variables//
	float relativeXPos, relativeYPos;
	//
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