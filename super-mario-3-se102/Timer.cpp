#include "Timer.h"
#include "PlayerVariables.h"

const int Timer::START_TIME = 300;

Timer::Timer() : time(0), stop(true)
{
}

void Timer::Start()
{
	stop = false;
	if (PlayerVariables::GetTime() == 0)
		PlayerVariables::SetTimer(START_TIME);
}

void Timer::Stop()
{
	stop = true;
}


void Timer::Update(float delta)
{
	if (stop)
		return;

	time += delta;
	if (time >= 0.75f) {
		time -= 0.75f;
		PlayerVariables::TickTimer();
	}
}
