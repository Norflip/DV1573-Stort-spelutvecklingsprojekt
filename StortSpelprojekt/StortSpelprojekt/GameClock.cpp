#include "stdafx.h"
#include "GameClock.h"
#include <iostream>
GameClock::GameClock()
{
	start = std::chrono::steady_clock::now();
	now = start;
	endSecond = now + std::chrono::seconds(1);
	frameCounter = 0;
	FPS = 0;
	active = true;
}

GameClock::~GameClock()
{
}

double GameClock::GetMiliseconds()const
{
	return ms;
}

double GameClock::GetMicroseconds()const
{
	return microS;
}

double GameClock::GetSeconds()const
{
	return seconds;
}

double GameClock::GetFrameTime()const
{
	return frameTime;
}

double GameClock::GetFramesPerSecond()const
{
	return FPS;
}

double GameClock::GetFramesPerSecondSmooth() const// not implemented and not needed (yet at least)
{

	return 0.0;
}

void GameClock::Update()
{
	ms = std::chrono::duration<double, std::milli>(now - start).count();
	microS = std::chrono::duration<double, std::micro>(now - start).count();
	seconds = std::chrono::duration<double, std::milli>(now - start).count() / 1000;
	frameTime = std::chrono::duration_cast<std::chrono::microseconds>(now - prev).count() / (double)1000;

	if (std::chrono::duration<double, std::milli>(now - endSecond).count() < 0)
		frameCounter++;
	else
	{
		FPS = frameCounter;
		frameCounter = 0;
		endSecond = now + std::chrono::seconds(1);
	}

	prev = now;
	now = std::chrono::steady_clock::now();
}
void GameClock::Restart()
{
	start = std::chrono::steady_clock::now();
}

bool GameClock::Stop()
{
	active = false;
	return active;
}

bool GameClock::Start()
{
	if (active)
		return false;
	else
	{
		start = std::chrono::steady_clock::now();
		active = true;
		return true;
	}


}