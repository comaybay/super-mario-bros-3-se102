#include "PlayerVariables.h"
#include "Utils.h"

const int PlayerVariables::MAX_POWER_LEVEL = 6;

PlayerPowerLevel PlayerVariables::playerPowerLevel = PlayerPowerLevel::SMALL;
int PlayerVariables::numberOfCoins = 0;
int PlayerVariables::numberOfLives = 0;
int PlayerVariables::powerMeterLevel = 0;
std::string PlayerVariables::standingWMNodeId = "NStart";

PlayerPowerLevel PlayerVariables::GetPlayerPowerLevel()
{
	return playerPowerLevel;
}

void PlayerVariables::SetPlayerPowerLevel(PlayerPowerLevel powerLevel)
{
	playerPowerLevel = powerLevel;
}

int PlayerVariables::GetNumberOfCoins()
{
	return numberOfCoins;
}

void PlayerVariables::SetNumberOfCoins(int num)
{
	numberOfCoins = num;
}

void PlayerVariables::AddToNumberOfCoins(int num)
{
	numberOfCoins += num;
}

int PlayerVariables::GetNumberOfLives()
{
	return numberOfLives;
}

void PlayerVariables::SetNumberOfLives(int num)
{
	numberOfLives = num;
}

void PlayerVariables::AddToNumberOfLives(int num)
{
	numberOfLives += num;
}

int PlayerVariables::GetPowerMeterLevel()
{
	return powerMeterLevel;
}
void PlayerVariables::SetPowerMeterLevel(int level)
{
	powerMeterLevel = level;
}

void PlayerVariables::AddToPowerMeterLevel(int level)
{
	powerMeterLevel += level;
	Utils::DebugOut(std::to_string(powerMeterLevel));
}

void PlayerVariables::SetStandingWMNodeId(const std::string& nodeId)
{
	standingWMNodeId = nodeId;
}

const std::string& PlayerVariables::GetStandingWMNodeId()
{
	return standingWMNodeId;
}
