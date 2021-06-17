#pragma once
#include <string>
#include "RectSequence.h"
#include <d3dx9.h>
#include <vector>
#include "Hitbox.h"
#include "Utils.h"

class Animation
{
public:
	Animation(
		std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, const std::vector<Utils::SpriteBox>& sequence,
		const Hitbox& hitbox
	);
	virtual void Render(Utils::Vector2<float> position);
	virtual void Update(float delta);
	Utils::SpriteBox GetCurrentSpriteBox();
	const Hitbox& GetHitbox();
	const std::string id;
protected:
	int currentFrame;
	float frameDuration;
	LPDIRECT3DTEXTURE9 texture;
	std::vector<Utils::SpriteBox> sequence;
	Hitbox hitbox;
private:
	float currentDuration;
};
typedef Animation* LPAnimation;

class AnimationId {
public:
	const static std::string NONE;
};

enum class AnimationType {
	NORMAL, FIXED
};
