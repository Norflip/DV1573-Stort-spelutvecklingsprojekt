#pragma once
#include "Scene.h"

class GUIFont;
class SpriteRenderPass;

ALIGN16
class GameOverScene : public Scene
{
private:

public:
	GameOverScene();
	virtual ~GameOverScene();

	void Initialize() override;
	void InitializeObjects() override;
	void InitializeGUI() override;

	void OnActivate() override;
	void OnDeactivate() override;

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;

	ALIGN16_ALLOC;

};