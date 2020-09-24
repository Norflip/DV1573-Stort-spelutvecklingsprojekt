#pragma once
#include <chrono>
//this should contain one timer from start to end
//Smoothly represent fps and frameTime
class GameClock
{
public:
	GameClock();
	~GameClock();
	double GetMiliseconds() const;
	double GetMicroseconds()const;
	double GetSeconds()const;
	double GetFrameTime()const;
	double GetFramesPerSecond()const;
	double GetFramesPerSecondSmooth()const; // not needed yet
	void Update();
	void Restart();
	bool Stop();
	bool Start();
	static GameClock& Instance()
	{
		static GameClock instance;
		return instance;
	}
	GameClock(GameClock const&) = delete;
	void operator=(GameClock const&) = delete;
private:
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> now;
	std::chrono::time_point<std::chrono::steady_clock> prev;
	std::chrono::time_point<std::chrono::steady_clock> endSecond;
	int frameCounter;
	int FPS;
	bool active;
};