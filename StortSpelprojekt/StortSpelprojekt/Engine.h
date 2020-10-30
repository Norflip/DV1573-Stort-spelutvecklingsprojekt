#pragma once
#include <chrono>
#include <thread>

#include "Window.h"
#include "Log.h"
//#include "Scene.h"

#include "GameScene.h"
#include "IntroScene.h"
#include "GameOverScene.h"
#include "WinScene.h"

constexpr int FIXED_FPS = 60;
constexpr float TARGET_FIXED_DELTA = 1.0f / FIXED_FPS;

#define MULTITHREAD_PHYSICS 0

class Engine
{

public:
	Engine(HINSTANCE hInstance);
	virtual ~Engine();

	void Run();
	void Exit();
	bool IsRunning() const { return this->running; }

	Scene* GetActiveScene() const { return this->activeScene; }
	void RegisterScene(size_t id, Scene* scene);
	void UnregisterScene(size_t id);
	void SwitchScene(size_t id);

	ALIGN16_ALLOC;

private:
	static void FixedUpdateLoop(Engine* engine);

private:
	bool running;
	std::unordered_map<size_t, Scene*> scenes;
	Scene* activeScene;
	Scene* previousScene;

	Window window;
	Renderer* renderer;

	ResourceManager* resourceManager;
};