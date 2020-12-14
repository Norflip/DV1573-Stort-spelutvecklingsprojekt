#pragma once
#include "Scene.h"
#include "World.h"

class GUIFont;
class GUICompass;
class SpriteRenderPass;

constexpr unsigned int LAST_SEGMENT = 3;
constexpr dx::XMFLOAT3 INTERIOR_POSITION = { 0.0f, -100.0f, 0.0f };

constexpr unsigned int POINTS_CLEARED_LEVEL = 100; //base currency earned from clearing one segment

ALIGN16
class GameScene : public Scene
{	
public:
	GameScene();
	virtual ~GameScene();
	
	void Initialize() override;
	void InitializeObjects() override;
	void InitializeGUI() override;
	void InitializeInterior();



	void OnActivate() override;
	void OnDeactivate() override;
	void SetSignPositions(SaveState& state);

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	void Render() override;

#if USE_IMGUI
	void OnIMGUIFrame() override;
#endif
	static bool immortal;
	static bool drawColliders;
	float VramUsage();
	float RamUsage();

	ALIGN16_ALLOC;
	
private:
	void TransitionToNextSegment();

	World world;
	Object* house;
	Particlesys* testParticles;

	Object* leftSign;
	Object* rightSign;
	Object* endSign;
	NodeWalkerComp* nodeWalker;
	ItemManager* items;
	float fogCol;
	int fogId;

	bool onceTest = true;
	bool end;
};