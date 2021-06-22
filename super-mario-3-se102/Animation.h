#pragma once
#include <string>
#include <d3dx9.h>
#include <vector>
#include "RectSequence.h"
#include "Hitbox.h"
#include "Utils.h"
#include "SpriteBox.h"

class Animation
{
public:
	Animation(std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, const std::vector<SpriteBox>& sequence);
	virtual void Render(Utils::Vector2<float> position);
	virtual void Update(float delta);
	SpriteBox GetCurrentSpriteBox();
	void SetAnimationSpeed(float speed);
	const std::string id;
protected:
	float animSpeed;
	int currentFrame;
	float frameDuration;
	LPDIRECT3DTEXTURE9 texture;
	std::vector<SpriteBox> sequence;
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
