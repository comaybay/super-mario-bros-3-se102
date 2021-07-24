#pragma once
class Timer
{
public:
	Timer();
	void Start();
	void Stop();
	void Update(float delta);

private:
	float time;
	bool stop;

	static const int START_TIME;

};

