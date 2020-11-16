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
}

void GUICompass::Update()
{

	sm::Vector3 playerPos = playerObj->GetTransform().GetPosition();
	sm::Vector3 housePos = houseObj->GetTransform().GetPosition();

	float distance = playerPos.Distance(playerPos, housePos);
	float scale = Math::Clamp((10 / pow(distance, 0.7f)), 0.4f, 1.f);
	house->SetScale(scale, scale);
	house->SetPosition((-house->GetWidth() / 2) + scale * (house->GetWidth() / 2), compassYpos + (house->GetHeight() / 2) - scale * (house->GetHeight() / 2));
	house->SetColor({ house->GetColor().x, house->GetColor().y, house->GetColor().z, scale });


	system("CLS");
	CameraComponent* cam = playerObj->GetComponent<PlayerComp>()->GetCamera();
	sm::Vector4 camRot = cam->GetOwner()->GetTransform().GetWorldRotation();

	sm::Vector3 H = houseObj->GetTransform().GetPosition();
	sm::Vector3 P = playerObj->GetTransform().GetPosition();



	float angleHouse = std::atan2f(H.x - P.x, H.z - P.z) * (180 / (Math::PI));
	std::cout << "ANGLE OF TO HOUSE" << angleHouse << std::endl;

	dx::XMFLOAT3 direction;
	dx::XMStoreFloat3(&direction, dx::XMVector3Rotate({ 0,0,1 }, camRot));
	float angleOfPlayer = atan2(direction.x, direction.z) * (180 / (Math::PI));
	std::cout << "ANGLE OF PLAYERS" << angleOfPlayer << std::endl;

	float resultAngle = angleOfPlayer - angleHouse;
	if (resultAngle > 360)
	{
		resultAngle -= 360;
	}
	if (resultAngle > 180)
	{
		resultAngle -= 360;
	}
	if (resultAngle < -360)
	{
		resultAngle += 360;
	}
	if (resultAngle < -180)
	{
		resultAngle += 360;
	}

	std::cout << std::endl << "RESULTANGLE" << resultAngle << std::endl;
	float movePos = 0;

	if (resultAngle > 0 && resultAngle < 90.f)
	{
		movePos = -(resultAngle)*4.44;
	}
	else if (resultAngle < 0 && resultAngle > -90.f)
	{
		movePos = -(resultAngle) * 4.44;
	}
	else
		movePos = 10000;
	house->Move({ movePos,0 });


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
