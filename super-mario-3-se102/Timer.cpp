#include "Timer.h"
#include "PlayerVariables.h"

const int Timer::START_TIME = 300;

Timer::Timer() : time(0)
{
	PlayerVariables::SetTimer(0);
}

void Timer::Start()
{
	PlayerVariables::SetTimer(START_TIME);
}

void Timer::Update(float delta)
{
	time += delta;
	if (time >= 0.5f) {
		time -= 0.5f;
		PlayerVariables::TickTimer();
	}
}
