#pragma once
class PlayerVariables {
public:
	static int GetNumberOfCoins();
	static void SetNumberOfCoins(int num);
	static void AddToNumberOfCoins(int num);

	static int GetNumberOfLives();
	static void SetNumberOfLives(int num);
	static void AddToNumberOfLives(int num);

	static float GetPowerMeterLevel();
	static void SetPowerMeterLevel(float level);
	static void AddToPowerMeterLevel(float level);

private:
	static int numberOfCoins;
	static int numberOfLives;
	static float powerMeterLevel;
};