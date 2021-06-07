#pragma once
#include <string>
#include "RectSequence.h"
#include <d3dx9.h>
#include <vector>
#include "Utils.h"

class Animation
{
public:
	Animation(std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, std::vector<Utils::SpriteBox> sequence);
	virtual void Render(Utils::Vector2 position);
	virtual void Update(float delta);
	Utils::SpriteBox GetCurrentSpriteBox();
	const std::string id;
private:
	int currentFrame;
	float frameDuration;
	float currentDuration;
	LPDIRECT3DTEXTURE9 texture;
	std::vector<Utils::SpriteBox> sequence;
};
typedef Animation* LPAnimation;

class AnimationId {
public:
	const static std::string NONE;
};
