#pragma once
#include <chrono>
#include <assert.h>
#include <unordered_map>
#include <thread>

#include "Window.h"
#include "Log.h"
#include "Scene.h"

namespace dx = DirectX;

constexpr int FIXED_FPS = 50;
constexpr float TARGET_FIXED_DELTA = 1.0f / FIXED_FPS;




class Engine
{

public:
	Engine(HINSTANCE hInstance);
	virtual ~Engine();

	void Run();
	void Exit();
	bool IsRunning() const { return this->running; }

	Scene* GetActiveScene() const { return this->activeScene; }
	void RegisterScene (size_t id, Scene* scene);
	void UnregisterScene(size_t id);
	void SwitchScene (size_t id);

private:
	std::thread fixedLoopThread;
	static void FixedUpdateLoop(Engine* engine);

private:
	bool running;
	std::unordered_map<size_t, Scene*> scenes;
	Scene* activeScene;

	Window window;
	Renderer renderer;
};