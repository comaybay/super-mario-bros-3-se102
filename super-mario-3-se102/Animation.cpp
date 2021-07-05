#include "Animation.h"
#include "Game.h"
#include "TextureManager.h"
using namespace Utils;

const std::string AnimationId::NONE = "__ANI_ID_NONE__";

Animation::Animation(std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, const std::vector<SpriteBox>& sequence)
	: id(id), frameDuration(frameDuration), texture(texture), sequence(sequence),
	currentFrame(0),
	currentDuration(0),
	animSpeed(1)
{}

void Animation::Update(float delta)
{
	if (frameDuration == -1.0f)
		return;

	currentDuration += delta * animSpeed;

	int i = (int)(currentDuration / frameDuration);
	currentDuration -= frameDuration * i;
	currentFrame = (currentFrame + i) % sequence.size();
}

const SpriteBox& Animation::GetCurrentSpriteBox()
{
	return sequence[currentFrame];
}

const int Animation::GetCurrentFrame()
{
	return currentFrame;
}

const std::string& Animation::GetId()
{
	return id;
}

void Animation::SetAnimationSpeed(float speed)
{
	animSpeed = speed;
}

void Animation::Render(const Vector2<float>& position)
{
	SpriteBox sb = sequence[currentFrame];
	Vector2<float> renderPos = { position.x + sb.offset.x, position.y + sb.offset.y };
	Game::Draw(texture, sb.rect, renderPos);

}



