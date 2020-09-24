#pragma once
#include <chrono>
//this should contain one timer from start to end
//Smoothly represent fps and frameTime
class GameClock
{
public:
	GameClock();
	~GameClock();
	double GetMiliseconds();
	double GetMicroseconds();
	double GetSeconds();
	double GetFortnights(); //very important
	double GetFrameTime();
	double GetFramesPerSecond();
	double GetFramesPerSecondSmooth();
	void UpdateClock();

	static GameClock& Instance()
	{
		static GameClock instance;
		return instance;
	}
	GameClock(GameClock const&) = delete;
	void operator=(GameClock const&) = delete;
private:
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::duration previous;
	std::chrono::high_resolution_clock::duration now;

	//FPS stuff
	std::chrono::high_resolution_clock::duration endSecond;
	int amountOfFrames;
};

