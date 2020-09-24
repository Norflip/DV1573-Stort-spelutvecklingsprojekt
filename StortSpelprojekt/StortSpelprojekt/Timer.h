#pragma once
#include <chrono>


class Timer
{

public:
	Timer();
	double GetSeconds();
	double GetMiliseconds();
	double GetMicroseconds();
	double GetFrameTime();
	double GetFramesPerSecond();
	void Restart();
	bool Stop();
	bool Start();
private:
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
	std::chrono::time_point<std::chrono::steady_clock> now;
};