#pragma once
#include "Entity.h"
#include "PlayerVariables.h"

#include <array>

class HUD
{
public:
	HUD(const Utils::Vector2<float>& position = { 0, 0 });
	void Update(float delta);
	void SetPosition(const Utils::Vector2<float>& position);
	void Render();
	std::vector<int> SplitNumberToDigits(int number);

	template <std::size_t ARRAY_SIZE>
	void UpdateNumberAnims(const std::array<LPAnimation, ARRAY_SIZE>& anims, int number, bool showZeroes);


private:
	LPAnimation hudBgAnim;
	LPAnimation boardAnim;
	LPAnimation powerButtonAnim;
	LPAnimation playerButtonAnim;
	LPAnimation worldNumberAnim;
	std::array<LPAnimation, 6> powerMeterTriangles;
	std::array<LPAnimation, 2> coinCountAnims;
	std::array<LPAnimation, 2> liveCountAnims;
	std::array<LPAnimation, 7> scoreCountAnims;
	std::array<LPAnimation, 3> timerAnims;

	Utils::Vector2<float> position;

	static const Utils::Vector2<int> BOARD_OFFSET;
	static const Utils::Vector2<int> POWER_METER_OFFSET;
	static const Utils::Vector2<int> POWER_BUTTON_OFFSET;
	static const Utils::Vector2<int> TRIANGLE_OFFSET;
	static const Utils::Vector2<int> PLAYER_BUTTON_OFFSET;
	static const Utils::Vector2<int> WORLD_NUMBER_OFFSET;
	static const Utils::Vector2<int> COIN_COUNT_OFFSET;
	static const Utils::Vector2<int> NUMBER_OFFSET;
	static const Utils::Vector2<int> LIVE_COUNT_OFFSET;
	static const Utils::Vector2<int> SCORE_COUNT_OFFSET;
	static const Utils::Vector2<int> TIMER_OFFSET;
};

template<std::size_t ARRAY_SIZE>
inline void HUD::UpdateNumberAnims(const std::array<LPAnimation, ARRAY_SIZE>& anims, int number, bool showZeroes)
{
	std::vector<int> digits = SplitNumberToDigits(number);
	size_t zeroCount = anims.size() - digits.size();

	for (size_t i = 0; i < zeroCount; i++)
		anims[i]->SetFrame(showZeroes ? 0 : 10);

	for (size_t i = zeroCount; i < anims.size(); i++)
		anims[i]->SetFrame(digits[i - zeroCount]);

	//leave last one to 0 instaed of nothing
	if (!showZeroes && (digits.size() == 0 || digits[digits.size() - 1] == 0))
		anims[anims.size() - 1]->SetFrame(0);

}