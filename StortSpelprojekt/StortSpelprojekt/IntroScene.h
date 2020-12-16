#pragma once
#include "Scene.h"
#include "AudioMaster.h"

constexpr int COST = 50;
constexpr float INCREASE = 0.2f;
constexpr float SCALE = INCREASE * 5;

class GUIFont;
class SpriteRenderPass;
ALIGN16
class IntroScene : public Scene
{
public:
	IntroScene();
	virtual ~IntroScene();

	void Initialize() override;
	void InitializeObjects() override;
	void InitializeGUI() override;

	void OnActivate() override;
	void OnDeactivate() override;

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	
	ALIGN16_ALLOC;

private:
	bool configChanged;
	float sensitivity;

	void ChangeBarColor();
};