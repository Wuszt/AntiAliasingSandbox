#include "Time.h"

LARGE_INTEGER Time::s_lastCounts;
LARGE_INTEGER Time::s_frequency;
float Time::s_currentTime;
float Time::s_currentDeltaTime;

void Time::Initialize()
{
	QueryPerformanceFrequency(&Time::s_frequency);
	QueryPerformanceCounter(&Time::s_lastCounts);
}

float Time::GetTime()
{
	return s_currentTime;
}

void Time::UpdateTime(bool stopped)
{
	LARGE_INTEGER currentCounts;

	QueryPerformanceCounter(&currentCounts);

	if (!stopped)
	{
		s_currentDeltaTime = (currentCounts.QuadPart - (double)Time::s_lastCounts.QuadPart) / Time::s_frequency.QuadPart;
		s_currentTime += s_currentDeltaTime;
	}

	s_lastCounts = currentCounts;
}

float Time::GetDeltaTime()
{
	return s_currentDeltaTime;
}
