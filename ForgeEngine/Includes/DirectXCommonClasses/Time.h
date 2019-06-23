#pragma once
#include <Windows.h>
class Time
{
public:
	static void Initialize();
	static float GetTime();
	static void UpdateTime(bool);
	static float GetDeltaTime();
private:
	Time();

	static LARGE_INTEGER s_lastCounts;
	static LARGE_INTEGER s_frequency;
	static float s_currentTime;
	static float s_currentDeltaTime;
};

