#include "stdafx.h"
#include "GUIManager.h"
#include "GUICompass.h"

//GUICompass::GUICompass(Renderer& renderer, std::string file, float xPos, float yPos, float layerDepth, DrawDirection dir, ClickFunction clickFunc, GuiGroup group) :GUISprite(renderer, file, xPos, yPos, layerDepth, dir, clickFunc, group)

GUICompass::GUICompass(Renderer& renderer, Window* window, Object* houseObj, Object* playerObj):houseObj(houseObj), playerObj(playerObj)
{
	this->renderer = &renderer;
	this->window = window;
	backgroundBar = new GUISprite(renderer, "Textures/Compass.png", 0, 00, 0.2f, DrawDirection::Center, ClickFunction::NotClickable);
	house = new GUISprite(renderer, "Textures/House.png", 0, 0, 0.0f, DrawDirection::Center, ClickFunction::NotClickable);
	backgroundBar->Move({ 0.f, 30.f });
	compassYpos = backgroundBar->GetYpos();
	compassXpos = backgroundBar->GetXpos();
}

void GUICompass::Update()
{
	sm::Vector3 playerPos = playerObj->GetTransform().GetPosition();
	sm::Vector3 housePos= houseObj->GetTransform().GetPosition();
	
	float distance = playerPos.Distance(playerPos, housePos);
	std::cout <<"pre "<< distance << std::endl;
	
	float scale  = Math::Clamp((10 / pow(distance,0.7f)),0.4f,1.f);
	std::cout << "post " << scale << std::endl;
	house->SetScale(scale, scale);
	//Places icon at centre with scaling taken into consideration
	house->SetPosition((-house->GetWidth() / 2) +scale*(house->GetWidth()/2), compassYpos+ (house->GetHeight()/2) - scale * (house->GetHeight()/2));
	house->Move({ 0,0 });
	//logic here
}

void GUICompass::Draw(DirectX::SpriteBatch* test)
{
	backgroundBar->Draw(test);
	house->Draw(test);
	//sfuelTest->Draw(test);
}

void GUICompass::SetPosition(float x, float y)
{
}
