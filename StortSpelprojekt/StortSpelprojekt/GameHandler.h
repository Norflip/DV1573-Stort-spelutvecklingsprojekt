#pragma once
#include "GameScene.h"

class GameHandler
{
public:
	GameHandler();
	virtual~GameHandler();

	void Initialize();

	void Update(const float& deltaTime);
	void FixedUpdate(const float& fixedDeltaTime);

	void SwitchScene();

private:
	GameScene* gScene;
};