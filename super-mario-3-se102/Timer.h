#pragma once
class Timer
{
public:
	Timer();
	void Start();
	void Update(float delta);

private:
	float time;

	static const int START_TIME;

};

