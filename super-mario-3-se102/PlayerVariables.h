#pragma once
#include "PlayerPowerLevel.h"
#include "GoalRouletteCard.h"
#include "Utils.h"
#include <string>
#include <unordered_map>
#include <array>

class PlayerVariables {
public:
	static PlayerPowerLevel GetPlayerPowerLevel();
	static void SetPlayerPowerLevel(PlayerPowerLevel powerLevel);

	static int GetTime();
	static void SetTimer(int time);
	static void TickTimer();

	static int GetScore();
	static void AddToScore(int num);

	static int GetNumberOfCoins();
	static void AddToNumberOfCoins(int num);

	static int GetNumberOfLives();
	static void AddToNumberOfLives(int num);

	static int GetPowerMeterValue();
	static void SetPowerMeter(int value);
	static void AddToPowerMeter(int value);

	static void AddCard(GoalRouletteCard card);
	static const std::array<GoalRouletteCard, 3>& GetCards();

	static int GetWorldMapNumber();
	static void SetWorldMapNumber(int number);

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
	static int score;
	static PlayerPowerLevel playerPowerLevel;
	static int numberOfCoins;
	static int numberOfLives;
	static int powerMeterLevel;
	static int worldMapNumber;
	static int timer;
	static std::array<GoalRouletteCard, 3> cards;
	static int cardPointer;
	static bool isInPipe;
	static bool isFlownBySuperNoteBlock;
	static std::string standingWMNodeId;
	static std::string prevScenePath;

	static const int MAX_SCORE;
	static const int MAX_COINS;
	static const int MAX_LIVES;
};