#include "Timer.h"
//General timer. Clock should be one of these
Timer::Timer()
{
	start = std::chrono::high_resolution_clock::now();
	stop = start;
}

double Timer::GetSeconds()
{
	return 0.0;
}

double Timer::GetMiliseconds()
{
	return 0.0;
}

double Timer::GetMicroseconds()
{
	return 0.0;
}

double Timer::GetFrameTime()
{
	return 0.0;
}

double Timer::GetFramesPerSecond()
{
	return 0.0;
}

void Timer::Restart()
{
}

bool Timer::Stop()
{
	return false;
}

bool Timer::Start()
{
	return false;
}
