#include "GameClock.h"
#include <iostream>
GameClock::GameClock()
{
	start = std::chrono::high_resolution_clock::now();
	now = start.time_since_epoch();
	endSecond = start;
	amountOfFrames = 0;
}

GameClock::~GameClock()
{
}

double GameClock::GetMiliseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

double GameClock::GetMicroseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}

double GameClock::GetSeconds()
{
	return std::chrono::duration_cast<std::chrono::seconds>(now).count();
}

double GameClock::GetFortnights()
{
	return std::chrono::duration_cast<std::chrono::hours>(now).count()/(24*14);
}

double GameClock::GetFrameTime()
{
	return  (((std::chrono::duration_cast<std::chrono::microseconds>(now - previous).count()/(double)1000)));
}

double GameClock::GetFramesPerSecond()
{

	//return 1/GetFrameTime()*1000;
	//return (int)( ((std::chrono::duration_cast<std::chrono::nanoseconds>(now - previous).count())));
	return amountOfFrames;
}

double GameClock::GetFramesPerSecondSmooth()
{

	return 0.0;
}

void GameClock::UpdateClock()
{
	previous = now;
	now = (std::chrono::high_resolution_clock::now() - start);


	if (std::chrono::duration_cast<std::chrono::milliseconds>(endSecond - now).count() >0)
	{
		amountOfFrames++;
	}
	else
	{
		endSecond = now;
		endSecond += std::chrono::milliseconds(1000);
		amountOfFrames = 0;
	}
	//endSecond = now + std::chrono::milliseconds(1000);
	//amountOfFrames = 0;
	
}
