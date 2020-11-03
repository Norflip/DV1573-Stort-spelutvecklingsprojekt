#pragma once
#include "Scene.h"
#include "AudioMaster.h"
class GUIFont;
class SpriteRenderPass;

ALIGN16
class IntroScene : public Scene
{
private:
	

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

};