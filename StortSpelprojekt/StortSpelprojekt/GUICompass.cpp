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
	system("CLS");
	sm::Vector3 playerPos = playerObj->GetTransform().GetPosition();
	sm::Vector3 housePos= houseObj->GetTransform().GetPosition();
	
	float distance = playerPos.Distance(playerPos, housePos);
	//std::cout <<"pre "<< distance << std::endl;
	
	float scale  = Math::Clamp((10 / pow(distance,0.7f)),0.4f,1.f);
	//std::cout << "post " << scale << std::endl;
	house->SetScale(scale, scale);
	//Places icon at centre with scaling taken into consideration
	house->SetPosition((-house->GetWidth() / 2) +scale*(house->GetWidth()/2), compassYpos+ (house->GetHeight()/2) - scale * (house->GetHeight()/2));
	house->SetColor({ house->GetColor().x, house->GetColor().y, house->GetColor().z, scale });
	
	// some logic for lookat house should be done here
	
	//GETS ROTATION OF PLAYER WITH FUCKERY. PLEASE FIX AN ACTUAL ROTATION OF THE PLAYER
	CameraComponent* cam = playerObj->GetComponent<PlayerComp>()->GetCamera();
	dx::XMVECTOR camRot = cam->GetOwner()->GetTransform().GetRotation();
	//camRot = cam->GetOwner()->GetTransform().TransformDirectionCustomRotation({1,0,0 }, camRot);
	sm::Vector3 test = playerObj->GetComponent<ControllerComp>()->GetGroundRot();
	//test = playerObj->GetComponent<RigidBodyComponent>()->GetRigidBody().get

	sm::Vector3 playerV = test;
	playerV.Normalize();
	std::cout << "X" << test.x << std::endl;
	std::cout << "Y" << test.y << std::endl;
	std::cout << "Z" << test.z << std::endl;
	sm::Vector3 H = houseObj->GetTransform().GetPosition();
	sm::Vector3 P = playerObj->GetTransform().GetPosition();
	sm::Vector3 vP2H;
	vP2H = P - H;
	vP2H.Normalize();
	
	float angle = std::atan2f(dx::XMVectorGetZ(camRot) -vP2H.z , dx::XMVectorGetX(camRot) - vP2H.x  ) * (180 / (Math::PI));
	
	float movePos = 0;
	//NOW.... IF ANGLE IS 90 -> 0 it should more to the left
	//angle = abs(angle);
	if (angle > 0 && angle < 90.f)
	{
		movePos = -(angle);
	}
	else if (angle < 360.f && angle > 270.f)
	{
		movePos = 360.f-(angle);
	}
	else
		movePos = 10000;
	// IF > 90 die



	//std::cout << test.m128_f32[0] << std::endl;
	std::cout <<"ANGLE"<< angle << std::endl;
	//std::cout<< playerV.z << std::endl;
	//std::cout << playerV.y << std::endl;
	//std::cout << playerV.x << std::endl;
	//sm::Vector3 houseV= houseObj->GetTransform().GetPosition();
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
