#pragma once
#include "Scene.h"
#define SCROLLING_SPEED 20
class GUIFont;
class SpriteRenderPass;

ALIGN16
class CreditsScene : public Scene
{
private:
	dx::XMFLOAT2 creditsPos;


	std::string GetCreditsString();
public:
	CreditsScene();
	virtual ~CreditsScene();

	void Initialize() override;
	void InitializeObjects() override;
	void InitializeGUI() override;

	void OnActivate() override;
	void OnDeactivate() override;

	void Update(const float& deltaTime) override;
	void Render() override;

	ALIGN16_ALLOC;

};