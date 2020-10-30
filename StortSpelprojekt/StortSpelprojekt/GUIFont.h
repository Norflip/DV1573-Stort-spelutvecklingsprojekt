#pragma once
#include "GUIManager.h"
#include "SpriteFont.h"
class GUIFont : public GUIObject
{
public:
	GUIFont();
	GUIFont(Renderer&, const std::string& display, float x = 0, float y = 0, GuiGroup group = GuiGroup::Default);
	~GUIFont();
	void Draw(DirectX::SpriteBatch*) override;
	void SetPosition(float x, float y) override;


	void SetFont(const wchar_t* font);
	void SetString(const std::string& text);
	void SetFontSize(DirectX::XMFLOAT2 fontSize);
	void SetFontColor(DirectX::XMVECTORF32 fontColor);
	std::string GetString() const;
	void Update() {};

private:
	Renderer* renderer;
	DirectX::SpriteFont* spriteFont;
	std::string text;
	std::wstring txtDisplay;
	float x, y;
	DirectX::XMFLOAT2 fontSize;
	DirectX::XMVECTORF32 fontColor;
};