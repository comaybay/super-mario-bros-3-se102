#include "PlayerVariables.h"
#include "Utils.h"

//TODO: REMOVE TEST VALUE
const int PlayerVariables::MAX_POWER_METER_VALUE = 2;

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
void PlayerVariables::SetPowerMeter(int level)
{
	powerMeterLevel = level;
}

void PlayerVariables::AddToPowerMeterLevel(int level)
{
	powerMeterLevel += level;
}

void PlayerVariables::SetStandingWMNodeId(const std::string& nodeId)
{
	standingWMNodeId = nodeId;
}

const std::string& PlayerVariables::GetStandingWMNodeId()
{
	return standingWMNodeId;
}
