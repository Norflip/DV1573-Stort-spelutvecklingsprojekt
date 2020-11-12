#include "stdafx.h"
#include "GUIManager.h"
#include "GUICompass.h"

//GUICompass::GUICompass(Renderer& renderer, std::string file, float xPos, float yPos, float layerDepth, DrawDirection dir, ClickFunction clickFunc, GuiGroup group) :GUISprite(renderer, file, xPos, yPos, layerDepth, dir, clickFunc, group)

GUICompass::GUICompass(Renderer& renderer, Window* window)
{
	this->renderer = &renderer;
	this->window = window;
	backgroundBar = new GUISprite(renderer, "Textures/Compass.png", 0, 10, 0.2f, DrawDirection::Center, ClickFunction::NotClickable);
	house = new GUISprite(renderer, "Textures/House.png", 0, 10, 0.0f, DrawDirection::Center, ClickFunction::NotClickable);
	backgroundBar->Move({ 0.f, 30.f });
}

void GUICompass::Update()
{
	backgroundBar->SetScale(1.f, 1.f);
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
