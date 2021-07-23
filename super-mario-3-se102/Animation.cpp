#include "Animation.h"
#include "Game.h"
#include "TextureManager.h"
using namespace Utils;

const std::string AnimationId::NONE = "__ANI_ID_NONE__";

Animation::Animation(std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, const std::vector<SpriteBox>& sequence, bool loopAnim)
	: id(id), frameDuration(frameDuration), texture(texture), sequence(sequence),
	currentFrame(0),
	currentDuration(0),
	animSpeed(1),
	updateHandler(loopAnim ? &Animation::LoopUpdate : &Animation::NoLoopUpdate)
{}

void Animation::SetLoopAnimation(float state) {
	updateHandler = state ? &Animation::LoopUpdate : &Animation::NoLoopUpdate;
}


void Animation::Update(float delta)
{
	if (frameDuration < 0)
		return;

	(this->*updateHandler)(delta);
}

void Animation::LoopUpdate(float delta) {
	currentDuration += delta * animSpeed;

	int i = int(currentDuration / frameDuration);
	currentDuration -= frameDuration * i;
	currentFrame = (currentFrame + i) % sequence.size();
}

void Animation::NoLoopUpdate(float delta) {
	if (currentFrame == sequence.size() - 1)
		return;

	currentDuration += delta * animSpeed;

	int i = int(currentDuration / frameDuration);
	currentDuration -= frameDuration * i;
	currentFrame = min(currentFrame + i, (int)sequence.size() - 1);
}

const SpriteBox& Animation::GetCurrentSpriteBox()
{
	return sequence[currentFrame];
}

const int Animation::GetCurrentFrame()
{
	return currentFrame;
}

void Animation::SetFrame(int frame)
{
	currentFrame = frame;
}

const std::string& Animation::GetId()
{
	return id;
}

bool Animation::Finished()
{
	return (updateHandler == &Animation::NoLoopUpdate && currentFrame == sequence.size() - 1);

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



