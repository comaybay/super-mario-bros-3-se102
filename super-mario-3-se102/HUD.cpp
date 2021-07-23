
#include "HUD.h"
#include "Game.h"
#include "AnimationManager.h"

#include <algorithm>

using namespace Utils;

const Vector2<int> HUD::BOARD_OFFSET = { 4, 4 };
const Vector2<int> HUD::POWER_METER_OFFSET = { BOARD_OFFSET.x + 52, BOARD_OFFSET.y + 7 };
const Vector2<int> HUD::TRIANGLE_OFFSET = { 8, 0 };
const Vector2<int> HUD::POWER_BUTTON_OFFSET = { POWER_METER_OFFSET.x + TRIANGLE_OFFSET.x * 6 + 1, POWER_METER_OFFSET.y };
const Vector2<int> HUD::PLAYER_BUTTON_OFFSET = { BOARD_OFFSET.x + 4, BOARD_OFFSET.y + 15 };
const Vector2<int> HUD::WORLD_NUMBER_OFFSET = { BOARD_OFFSET.x + 36, BOARD_OFFSET.y + 6 };
const Vector2<int> HUD::COIN_COUNT_OFFSET = { BOARD_OFFSET.x + 132, BOARD_OFFSET.y + 6 };
const Vector2<int> HUD::NUMBER_OFFSET = { 8, 0 };
const Vector2<int> HUD::LIVE_COUNT_OFFSET = { BOARD_OFFSET.x + 28, BOARD_OFFSET.y + 14 };
const Vector2<int> HUD::SCORE_COUNT_OFFSET = { BOARD_OFFSET.x + 52, BOARD_OFFSET.y + 14 };
const Vector2<int> HUD::TIMER_OFFSET = { BOARD_OFFSET.x + 124, BOARD_OFFSET.y + 14 };


HUD::HUD(const Vector2<float>& position) :
	position(position),
	hudBgAnim(AnimationManager::GetNew("HUDBackground")),
	boardAnim(AnimationManager::GetNew("HUDBoard")),
	powerButtonAnim(AnimationManager::GetNew("HUDPowerButton")),
	playerButtonAnim(AnimationManager::GetNew("HUDMarioButton")),
	worldNumberAnim(AnimationManager::GetNew("Numbers"))
{
	for (size_t i = 0; i < powerMeterTriangles.size(); i++)
		powerMeterTriangles[i] = AnimationManager::GetNew("HUDPowerTriangle");

	for (size_t i = 0; i < coinCountAnims.size(); i++)
		coinCountAnims[i] = AnimationManager::GetNew("Numbers");

	for (size_t i = 0; i < liveCountAnims.size(); i++)
		liveCountAnims[i] = AnimationManager::GetNew("Numbers");

	for (size_t i = 0; i < scoreCountAnims.size(); i++)
		scoreCountAnims[i] = AnimationManager::GetNew("Numbers");

	for (size_t i = 0; i < timerAnims.size(); i++)
		timerAnims[i] = AnimationManager::GetNew("Numbers");

	worldNumberAnim->SetFrame(PlayerVariables::GetWorldMapNumber());
}

void HUD::Update(float delta)
{
	int powerMeterValue = PlayerVariables::GetPowerMeterValue();

	for (int i = 0; i < powerMeterValue; i++)
		powerMeterTriangles[i]->SetFrame(0);

	for (int i = powerMeterValue; i < PlayerVariables::MAX_POWER_METER_VALUE; i++)
		powerMeterTriangles[i]->SetFrame(1);

	if (powerMeterValue == PlayerVariables::MAX_POWER_METER_VALUE)
		powerButtonAnim->Update(delta);
	else
		powerButtonAnim->SetFrame(1);

	UpdateNumberAnims(scoreCountAnims, PlayerVariables::GetScore(), true);
	UpdateNumberAnims(coinCountAnims, PlayerVariables::GetNumberOfCoins(), false);
	UpdateNumberAnims(liveCountAnims, PlayerVariables::GetNumberOfLives(), false);
	UpdateNumberAnims(timerAnims, PlayerVariables::GetTime(), true);
}

void HUD::SetPosition(const Vector2<float>& position)
{
	this->position = position;
}

void HUD::Render()
{
	Game::DrawHUD(hudBgAnim->GetTexture(), hudBgAnim->GetCurrentSpriteBox().rect, position);
	Game::DrawHUD(boardAnim->GetTexture(), boardAnim->GetCurrentSpriteBox().rect, position + BOARD_OFFSET);
	Game::DrawHUD(powerButtonAnim->GetTexture(), powerButtonAnim->GetCurrentSpriteBox().rect, position + POWER_BUTTON_OFFSET);
	Game::DrawHUD(playerButtonAnim->GetTexture(), playerButtonAnim->GetCurrentSpriteBox().rect, position + PLAYER_BUTTON_OFFSET);
	Game::DrawHUD(worldNumberAnim->GetTexture(), worldNumberAnim->GetCurrentSpriteBox().rect, position + WORLD_NUMBER_OFFSET);

	for (int i = 0; i < static_cast<float>(powerMeterTriangles.size()); i++)
		Game::DrawHUD(powerMeterTriangles[i]->GetTexture(), powerMeterTriangles[i]->GetCurrentSpriteBox().rect, position + POWER_METER_OFFSET + TRIANGLE_OFFSET * i);

	for (int i = 0; i < static_cast<float>(coinCountAnims.size()); i++)
		Game::DrawHUD(coinCountAnims[i]->GetTexture(), coinCountAnims[i]->GetCurrentSpriteBox().rect, position + COIN_COUNT_OFFSET + NUMBER_OFFSET * i);

	for (int i = 0; i < static_cast<float>(liveCountAnims.size()); i++)
		Game::DrawHUD(liveCountAnims[i]->GetTexture(), liveCountAnims[i]->GetCurrentSpriteBox().rect, position + LIVE_COUNT_OFFSET + NUMBER_OFFSET * i);

	for (int i = 0; i < static_cast<float>(scoreCountAnims.size()); i++)
		Game::DrawHUD(scoreCountAnims[i]->GetTexture(), scoreCountAnims[i]->GetCurrentSpriteBox().rect, position + SCORE_COUNT_OFFSET + NUMBER_OFFSET * i);

	for (int i = 0; i < static_cast<float>(timerAnims.size()); i++)
		Game::DrawHUD(timerAnims[i]->GetTexture(), timerAnims[i]->GetCurrentSpriteBox().rect, position + TIMER_OFFSET + NUMBER_OFFSET * i);
}

std::vector<int> HUD::SplitNumberToDigits(int number)
{
	std::vector<int> digits;

	while (number != 0) {
		digits.push_back(number % 10);
		number /= 10;
	}

	std::reverse(digits.begin(), digits.end());
	return digits;
}
