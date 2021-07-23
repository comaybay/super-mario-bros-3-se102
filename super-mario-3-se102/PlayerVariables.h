#pragma once
#include "PlayerPowerLevel.h"
#include "Utils.h"
#include <string>
#include <unordered_map>

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

	static void SetIsInPipe(bool state);
	static bool IsInPipe();

	static void SetIsFlownBySuperNoteBlock(bool state);
	static bool IsFlownBySuperNoteBlock();

	static const std::string& GetPreviousScenePath();

	/// <summary>
	/// Used internally by Game every time there is scene switching.
	/// </summary>
	static void _SetPreviousScenePath(const std::string& scenePath);

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
	static bool isInPipe;
	static bool isFlownBySuperNoteBlock;
	static std::string standingWMNodeId;
	static std::string prevScenePath;
};