#include "GUIManager.h"
#include "SpriteFont.h"
#include "GUIFont.h"

GUIFont::GUIFont()
{
	this->renderer = nullptr;
	this->spriteFont = 0;
	this->text = "";
	this->txtDisplay = L"";
	this->x = 0.0f;
	this->y = 0.0f;
	this->fontColor = DirectX::Colors::White;
	this->fontSize = DirectX::XMFLOAT2(1.0f, 1.0f);
}

GUIFont::GUIFont(Renderer& renderer, const std::string& display, float x, float y,GuiGroup group)
{
	this->x = x;
	this->y = y;
	this->SetString(display);
	this->renderer = &renderer;
	this->spriteFont = new DirectX::SpriteFont(this->renderer->GetDevice(), L"SpriteFonts/TheWanters24.spritefont");
	//this->spriteFont = new DirectX::SpriteFont(this->renderer->GetDevice(), L"SpriteFonts/comic_sans_ms_16.spritefont");
	this->fontColor = DirectX::Colors::White;
	this->fontSize = DirectX::XMFLOAT2(1.f, 1.f);
	this->group = group;
}

GUIFont::~GUIFont()
{
}

void GUIFont::Draw(DirectX::SpriteBatch* spriteBatch)
{

	if (this->GetVisible())
	{
		spriteFont->DrawString(
			spriteBatch,
			txtDisplay.c_str(),
			DirectX::XMFLOAT2(x, y),
			this->fontColor,
			0.0f, //ROT
			DirectX::XMFLOAT2(0.0f, 0.0f), fontSize //Size
		);
	}
}

void GUIFont::SetPosition(float x, float y)
{
	this->x = x;
	this->y = y;
}

void GUIFont::SetFont(const wchar_t* font)
{
	this->spriteFont = new DirectX::SpriteFont(this->renderer->GetDevice(), font);
}

void GUIFont::SetString(const std::string& text)
{
	txtDisplay = std::wstring(text.begin(), text.end());
	this->text = text;
}

void GUIFont::SetFontSize(DirectX::XMFLOAT2 fontSize)
{
	this->fontSize = fontSize;
}

void GUIFont::SetFontColor(DirectX::XMVECTORF32 fontColor)
{
	this->fontColor = fontColor;
}

std::string GUIFont::GetString() const
{
	return std::string();
}
