#pragma once
#include <chrono>
#include <thread>

#include "Window.h"
#include "Scene.h"

#include "GameScene.h"
#include "IntroScene.h"
#include "GameOverScene.h"
#include "WinScene.h"
#include "CreditsScene.h"

constexpr int FIXED_FPS = 60;
constexpr float TARGET_FIXED_DELTA = 1.0f / FIXED_FPS;

enum SceneIndex { INTRO = 0, GAME_OVER = 1, GAME = 2, WIN = 3, CREDITS = 4 };
constexpr size_t SCENE_COUNT = 5;

enum DayTime {DAY =0, DUSK = 1, NIGHT = 2, END = 3};

class Engine
{
public:
	Engine(HINSTANCE hInstance);
	virtual ~Engine();

	void Run();
	void Exit();
	bool IsRunning() const { return this->running; }

	Scene* GetActiveScene() const;
	void RegisterScene(size_t id, Scene* scene);
	void UnregisterScene(size_t id);
	void SwitchScene(size_t id);
	
	Physics* GetPhysics() const { return this->physics; }
	ResourceManager* GetResources () const { return this->resourceManager; }

	void Pause(bool pause) { this->pause = pause; }
	bool IsPaused() const { return pause; }

	ALIGN16_ALLOC;
	static Engine* Instance;

private:
	static void FixedUpdateLoop(Engine* engine);

private:
	bool running;
	Scene* scenes[SCENE_COUNT];
	int activeSceneIndex;
	bool pause;

	Window window;
	Renderer* renderer;
	Physics* physics;
	ResourceManager* resourceManager;
	size_t sceneSwitch;
};