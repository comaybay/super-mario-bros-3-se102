#include "PlayerVariables.h"
#include "Utils.h"

const int PlayerVariables::MAX_POWER_METER_VALUE = 6;
const int PlayerVariables::MAX_SCORE = 9999999;
const int PlayerVariables::MAX_COINS = 99;
const int PlayerVariables::MAX_LIVES = 99;

PlayerPowerLevel PlayerVariables::playerPowerLevel = PlayerPowerLevel::SMALL;
int PlayerVariables::score = 0;
int PlayerVariables::timer = 0;
int PlayerVariables::numberOfCoins = 0;
int PlayerVariables::numberOfLives = 4;
int PlayerVariables::powerMeterLevel = 0;
int PlayerVariables::worldMapNumber = 1;
std::array<GoalRouletteCard, 3> PlayerVariables::cards = { GoalRouletteCard::NONE, GoalRouletteCard::NONE, GoalRouletteCard::NONE, };
int PlayerVariables::cardPointer = 0;
bool PlayerVariables::isInPipe = false;
bool PlayerVariables::isFlownBySuperNoteBlock = false;
std::string PlayerVariables::standingWMNodeId = "NStart";
std::string PlayerVariables::prevScenePath = "";

PlayerPowerLevel PlayerVariables::GetPlayerPowerLevel()
{
	return playerPowerLevel;
}

void PlayerVariables::SetPlayerPowerLevel(PlayerPowerLevel powerLevel)
{
	playerPowerLevel = powerLevel;
}

int PlayerVariables::GetTime()
{
	return timer;
}

void PlayerVariables::SetTimer(int time)
{
	timer = time > 0 ? time : 0;
}

void PlayerVariables::TickTimer()
{
	timer = timer - 1 > 0 ? timer - 1 : 0;
}

int PlayerVariables::GetScore()
{
	return score;
}

void PlayerVariables::AddToScore(int num) {
	score = (score + num) < MAX_SCORE ? (score + num) : MAX_SCORE;
}


int PlayerVariables::GetNumberOfCoins() {
	return numberOfCoins;
}


void PlayerVariables::AddToNumberOfCoins(int num)
{
	numberOfCoins += num;
	if (numberOfCoins > MAX_COINS) {
		AddToNumberOfLives(1);
		numberOfCoins -= MAX_COINS + 1;
	}
}

int PlayerVariables::GetNumberOfLives()
{
	return numberOfLives;
}

void PlayerVariables::AddToNumberOfLives(int num)
{
	numberOfLives = (numberOfLives + num) < MAX_LIVES ? numberOfLives : MAX_LIVES;
}

int PlayerVariables::GetPowerMeterValue()
{
	return powerMeterLevel;
}

void PlayerVariables::SetPowerMeter(int value)
{
	powerMeterLevel = value < MAX_POWER_METER_VALUE ? value : MAX_POWER_METER_VALUE;

}

void PlayerVariables::AddToPowerMeter(int level)
{
	powerMeterLevel = (powerMeterLevel + level) < MAX_POWER_METER_VALUE ? (powerMeterLevel + level) : MAX_POWER_METER_VALUE;
}

void PlayerVariables::AddCard(GoalRouletteCard card)
{
	cards[cardPointer] = card;
	cardPointer = (cardPointer + 1) % 3;
}

const std::array<GoalRouletteCard, 3>& PlayerVariables::GetCards()
{
	return cards;
}

int PlayerVariables::GetWorldMapNumber()
{
	return worldMapNumber;
}

void PlayerVariables::SetWorldMapNumber(int number)
{
	worldMapNumber = number;
}

void PlayerVariables::SetIsInPipe(bool state)
{
	isInPipe = state;
}

bool PlayerVariables::IsInPipe()
{
	return isInPipe;
}

void PlayerVariables::SetIsFlownBySuperNoteBlock(bool state)
{
	isFlownBySuperNoteBlock = state;
}

bool PlayerVariables::IsFlownBySuperNoteBlock() {
	return isFlownBySuperNoteBlock;
}


const std::string& PlayerVariables::GetPreviousScenePath()
{
	return prevScenePath;
}

void PlayerVariables::_SetPreviousScenePath(const std::string& scenePath) {
	prevScenePath = scenePath;
}


void PlayerVariables::SetStandingWMNodeId(const std::string& nodeId)
{
	standingWMNodeId = nodeId;
}

const std::string& PlayerVariables::GetStandingWMNodeId()
{
	return standingWMNodeId;
}
