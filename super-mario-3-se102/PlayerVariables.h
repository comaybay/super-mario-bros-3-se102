#pragma once
#include "PlayerPowerLevel.h"

class PlayerVariables {
public:
	static PlayerPowerLevel GetPlayerPowerLevel();
	static void SetPlayerPowerLevel(PlayerPowerLevel powerLevel);

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
	static PlayerPowerLevel playerPowerLevel;
	static int numberOfCoins;
	static int numberOfLives;
	static float powerMeterLevel;
};