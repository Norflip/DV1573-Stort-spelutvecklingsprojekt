#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include "Object.h"
namespace dx = DirectX;
class MoveComponent:public Component
{
private:
	dx::XMVECTOR direction;
public:
	MoveComponent();

	virtual ~MoveComponent();

	void Move(DirectX::XMFLOAT3 moveVector);

	void Update(const float& deltaTime);

	/*
	move = cameraObject->AddComponent<MoveComponent>();
	objects.push_back(cameraObject);
	*/


};