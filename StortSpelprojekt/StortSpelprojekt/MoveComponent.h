#pragma once
#include "Component.h"
#include "Object.h"
//#include <DirectXMath.h>
namespace dx = DirectX;
class MoveComponent:public Component
{
private:
	dx::XMFLOAT3 direction;

public:
	MoveComponent();
	virtual ~MoveComponent();

	//void Move(DirectX::XMFLOAT3 moveVector);
	void Update(const float& deltaTime);

	/*
	move = cameraObject->AddComponent<MoveComponent>();
	objects.push_back(cameraObject);
	*/

};