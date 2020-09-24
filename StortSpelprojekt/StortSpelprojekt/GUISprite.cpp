#include "GUISprite.h"

GUISprite::GUISprite(ID3D11Device* device, std::string, float xPos, float yPos)
{
	// position
	this->xPos = xPos;
	this->xPos = yPos;
	this->position = dx::XMVectorSet(this->xPos, this->yPos, 0, 0);
	//Scale
	this->xScale = 1.0;
	this->yScale = 1.0;
	this->scale = dx::XMVectorSet(this->xScale, this->yScale, 0, 0);

	this->rotation = 0.0f;
	this->color = dx::XMVectorSet(1.f, 1.f, 1.f, 1.f);
	this->origin = dx::XMVectorSet(1.f, 1.f, 1.f, 1.f);


}

GUISprite::~GUISprite()
{
}

void GUISprite::Draw(dx::SpriteBatch* spriteBatch)
{
	
}

void GUISprite::SetPosition(float xPos, float yPos)
{
	this->xPos = xPos;
	this->yPos = yPos;
	this->position = dx::XMVectorSet(this->xPos, this->yPos, 0, 0);
}
