#pragma once
#include "PlayerPowerLevel.h"
#include <string>

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

	static int GetPowerMeterLevel();
	static void SetPowerMeter(int level);
	static void AddToPowerMeterLevel(int level);

	/// <summary>
	/// Set world map node id. This will be used to determine where player is at when loading a world map.
	/// </summary>
	static void SetStandingWMNodeId(const std::string& nodeId);

	/// <summary>
	/// Get standing world map node id. Default is NStart
	/// </summary>
	static const std::string& GetStandingWMNodeId();

	static const int MAX_POWER_METER_VALUE;

private:
	static PlayerPowerLevel playerPowerLevel;
	static int numberOfCoins;
	static int numberOfLives;
	static int powerMeterLevel;
	static std::string standingWMNodeId;
};