#include "stdafx.h"
#include "GUIManager.h"
#include "GUISprite.h"


GUISprite::GUISprite()
{
}

GUISprite::GUISprite(Renderer& renderer , std::string path, float xPos, float yPos,float layerDepth, DrawDirection dir, ClickFunction clickFunc, GuiGroup group)
{
	this->renderer = &renderer;
	// position
	this->clickFunc = clickFunc;
	//Scale
	this->xScale = 1.0;
	this->yScale = 1.0;
	this->direction = dir;
	this->group = group;
	this->rotation = 0.0f;
	this->baseColor = dx::XMVectorSet(1.f, 1.f, 1.f, 1.f);
	this->activeColor = dx::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f); // default bluetinted
	this->origin = dx::XMVectorSet(1.f, 1.f, 1.f, 1.f);
	this->SRV = nullptr;
	this->xPos = renderer.GetOutputWindow()->GetWidth()-xPos;
	this->spriteBatch = nullptr;
	this->filePath = path;
	this->layerDepth = layerDepth;
	this->SetVisible(true);
	this->scale = dx::XMVectorSet(this->xScale, this->yScale, 0, 0);

	Microsoft::WRL::ComPtr<ID3D11Resource> res;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;

	HRESULT result;
	std::wstring wsConvert(filePath.begin(), filePath.end());
	spriteBatch = std::make_unique<dx::SpriteBatch>(renderer.GetContext());

	// for testing purposes
	std::size_t found = path.find("Compass.dds");
	if (found != std::string::npos)
		result = DirectX::CreateDDSTextureFromFile(renderer.GetDevice(), wsConvert.c_str(), &res, &SRV);
	else
		result = DirectX::CreateWICTextureFromFile(renderer.GetDevice(), wsConvert.c_str(), &res, &SRV);
		
				//DirectX::SaveWICTextureToFile(renderer.GetContext(),res.Get(),GUID_ContainerFormatPng, L"SCREENSHOT.png");

	assert(SUCCEEDED(result));
	result = res.As(&tex);

	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);

	width = desc.Width;
	height = desc.Height;
	assert(SUCCEEDED(result));

	//directions
	SetPos(xPos, yPos, dir);

}

GUISprite::~GUISprite()
{
	if (SRV != nullptr) {
		SRV->Release();
		SRV = 0;
	}

	if (spriteBatch != nullptr) {
		spriteBatch = nullptr;
		spriteBatch = 0;
	}
}

void GUISprite::Draw(DirectX::SpriteBatch* test)
{
	if (this->GetVisible())
	{
		if (this->GetActivated())
			test->Draw(SRV, this->position, nullptr, this->activeColor, rotation, origin, scale, DirectX::SpriteEffects::SpriteEffects_None, this->layerDepth);
		else
			test->Draw(SRV, this->position, nullptr, this->baseColor, rotation, origin, scale, DirectX::SpriteEffects::SpriteEffects_None, this->layerDepth);
	}
}

void GUISprite::Draw()
{
	spriteBatch->Draw(SRV, this->position, nullptr, this->baseColor, rotation, origin, scale, DirectX::SpriteEffects::SpriteEffects_None, this->layerDepth);
}

void GUISprite::SetPosition(float xPos, float yPos)
{
	SetPos(xPos, yPos,direction);
}

void GUISprite::SetWICSprite(ID3D11Device* device,std::string spriteFile)
{
	if (this->SRV != nullptr)
		this->SRV->Release();

	HRESULT result;
	std::wstring wsConvert(spriteFile.begin(), spriteFile.end());
	result = DirectX::CreateWICTextureFromFile(device, wsConvert.c_str(), nullptr, &SRV);
	assert(SUCCEEDED(result));
}

void GUISprite::SetColor(sm::Vector4 color)
{
	this->baseColor = color;
}


void GUISprite::SetActiveColor(dx::XMVECTOR vector)
{
	this->activeColor = vector;
}

void GUISprite::SetScaleColor(float value)
{
	this->baseColor = { value, 1.0f -value, 0, 1.0f };
}

void GUISprite::SetDDSSprite(ID3D11Device* device,  std::string spriteFile)
{
	//if (this->SRV != nullptr)
	//	this->SRV->Release();

	//HRESULT result;
	//std::wstring wsConvert(spriteFile.begin(), spriteFile.end());
	//result = DirectX::CreateDDSTextureFromFile(device, wsConvert.c_str(), nullptr, &SRV);
	//assert(SUCCEEDED(result));
}

void GUISprite::SetScale(float x, float y)
{
	this->xScale = x;
	this->yScale = y;

	this->scale = dx::XMVectorSet(this->xScale, this->yScale, 0, 0);
}

void GUISprite::SetScaleBars(float yValue)
{
	if (yScale <= 1.0f)
		this->yScale = yValue;
	else if (yScale > 1.0f)
		yScale = 1.0f;
	/*else
		yScale = 1.0f;*/

	this->scale = dx::XMVectorSet(this->xScale, this->yScale, 0, 0);
}

void GUISprite::Move(POINTFLOAT moveDir)
{
	//Fix this after meeting
	this->position= dx::XMVectorSet(this->xPos+moveDir.x, this->yPos + moveDir.y, 0, 0);
	this->yPos = yPos + moveDir.y;
}

bool GUISprite::IsClicked()
{
	if(this->GetVisible())
	{
		if (clickFunc == ClickFunction::Clickable && IsMouseOver() && Input::Instance().GetLeftMouseKeyDown())
			return true;		
	}
	return false;
}

bool GUISprite::IsMouseOver()
{
	if (this->GetVisible())
	{
		if ((Input::Instance().GetMousePos().x > xPos && Input::Instance().GetMousePos().x < xPos + width) && (Input::Instance().GetMousePos().y > yPos && Input::Instance().GetMousePos().y < yPos + height))
		{
			SetActivated(true);
			return true;
		}
		else
		{
			SetActivated(false);
			return false;
		}
	}

	return false;
}

void GUISprite::Update()
{
	if (clickFunc == ClickFunction::Clickable && IsMouseOver() && Input::Instance().GetLeftMouseKeyDown())
		std::cout << "Do function" << std::endl;
}

void GUISprite::SetPos(float xPos, float yPos, DrawDirection dir)
{
	switch (dir)
	{
	case Default:
		this->xPos = xPos;
		this->yPos = yPos;
		this->relativeXPos = xPos;
		this->relativeYPos = yPos;
		break;
	case TopRight:
		this->xPos = renderer->GetOutputWindow()->GetWidth() - width * 2 - xPos;
		this->yPos = yPos;
		this->relativeXPos = renderer->GetOutputWindow()->GetWidth() - xPos - width;
		this->relativeYPos = yPos;
		break;
	case BottomLeft:
		this->xPos = xPos;
		this->yPos = renderer->GetOutputWindow()->GetHeight() - height * 2 - yPos;
		this->relativeXPos = xPos;
		this->relativeYPos = renderer->GetOutputWindow()->GetHeight() - yPos - height;
		break;
	case BottomRight:
		this->xPos = renderer->GetOutputWindow()->GetWidth() - width * 2 - xPos;
		this->yPos = renderer->GetOutputWindow()->GetHeight() - height * 2 - yPos;
		this->relativeXPos = renderer->GetOutputWindow()->GetWidth() - xPos - width;
		this->relativeYPos = renderer->GetOutputWindow()->GetHeight() - yPos - height;
		break;
	case Center:
		this->xPos = renderer->GetOutputWindow()->GetWidth() / 2 - width /2 - xPos;
		this->yPos = yPos;
		this->relativeXPos = renderer->GetOutputWindow()->GetWidth() / 2 - xPos - width/2;
		this->relativeYPos = yPos;	
		break;
	default:
		this->xPos = xPos;
		this->yPos = yPos;
		this->relativeXPos = xPos;
		this->relativeYPos = yPos;
		break;
	}
	this->position = dx::XMVectorSet(this->relativeXPos, this->relativeYPos, 0, 0);
}
