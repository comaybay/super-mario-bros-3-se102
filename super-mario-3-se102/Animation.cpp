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

SpriteBox Animation::GetCurrentSpriteBox()
{
	return sequence[currentFrame];
}

void Animation::SetAnimationSpeed(float speed)
{
	animSpeed = speed;
}

void Animation::Render(Vector2<float> position)
{
	SpriteBox sb = sequence[currentFrame];
	Vector2<int> cp = Game::GetActiveScene()->GetCameraPosition().Rounded();
	D3DXVECTOR3 p(round(position.x + sb.offset.x) - cp.x, round(position.y + sb.offset.y) - cp.y, 0);

	Game::GetD3DXSprite()->Draw(texture, &sb.rect, NULL, &p, D3DCOLOR_XRGB(255, 255, 255));
}

