#include "stdafx.h"
#include "GUIManager.h"
#include "GUICompass.h"

//GUICompass::GUICompass(Renderer& renderer, std::string file, float xPos, float yPos, float layerDepth, DrawDirection dir, ClickFunction clickFunc, GuiGroup group) :GUISprite(renderer, file, xPos, yPos, layerDepth, dir, clickFunc, group)

GUICompass::GUICompass(Renderer& renderer, Window* window, Object* houseObj, Object* playerObj) :houseObj(houseObj), playerObj(playerObj)
{
	this->renderer = &renderer;
	this->window = window;
	backgroundBar = new GUISprite(renderer, "Textures/Compass.png", 0, 00, 0.2f, DrawDirection::Center, ClickFunction::NotClickable);
	house = new GUISprite(renderer, "Textures/House.png", 0, 0, 0.0f, DrawDirection::Center, ClickFunction::NotClickable);
	backgroundBar->Move({ 0.f, 30.f });
	compassYpos = backgroundBar->GetYpos();
	compassXpos = backgroundBar->GetXpos();

	cam = playerObj->GetComponent<PlayerComp>()->GetCamera();
}
void GUICompass::Update()
{
	//Update Compass variables         
	sm::Vector4 camRot = cam->GetOwner()->GetTransform().GetWorldRotation();
	sm::Vector3 housePos = houseObj->GetTransform().GetPosition();
	sm::Vector3 playerPos = playerObj->GetTransform().GetPosition();
	playerPos = playerObj->GetTransform().GetPosition();
	housePos = houseObj->GetTransform().GetPosition();
	////////////////////////////////////////////////////////
	float distance = playerPos.Distance(playerPos, housePos);
	//Clamps scale between 0.4 and 1
	float scale = Math::Clamp((10 / pow(distance, 0.7f)), 0.4f, 1.f);
	house->SetScale(scale, scale);
	house->SetPosition((-house->GetWidth() / 2) + scale * (house->GetWidth() / 2), compassYpos + (house->GetHeight() / 2) - scale * (house->GetHeight() / 2));
	house->SetColor({ house->GetColor().x, house->GetColor().y, house->GetColor().z, scale });

	float angleHouse = std::atan2f(housePos.x - playerPos.x, housePos.z - playerPos.z) * (180 / (Math::PI));

	dx::XMFLOAT3 direction;
	dx::XMStoreFloat3(&direction, dx::XMVector3Rotate({ 0,0,1 }, camRot));
	float angleOfPlayer = atan2(direction.x, direction.z) * (180 / (Math::PI));

	float resultAngle = angleOfPlayer - angleHouse;

	//rotate angle fix
	if (resultAngle > 180)
		resultAngle -= 360;
	if (resultAngle < -180)
		resultAngle += 360;
	float movePos = -(resultAngle)*window->GetWidth() / 90;

	if (abs(movePos)+house->GetWidth() > (backgroundBar->GetWidth() / 2))
		movePos = 10000;
	house->Move({ movePos,0 });
}

void GUICompass::Draw(DirectX::SpriteBatch* test)
{
	backgroundBar->Draw(test);
	house->Draw(test);
}

void GUICompass::SetPosition(float x, float y)
{
}
