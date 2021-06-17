#include "Animation.h"
#include "Game.h"
#include "TextureManager.h"

const std::string AnimationId::NONE = "__ANI_ID_NONE__";

Animation::Animation(
	std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, const std::vector<Utils::SpriteBox>& sequence,
	const Hitbox& hitbox
)
	: id(id), frameDuration(frameDuration), texture(texture), sequence(sequence),
	hitbox(hitbox),
	currentFrame(0),
	currentDuration(0)
{}

void Animation::Update(float delta)
{
	if (frameDuration == -1.0f)
		return;

	currentDuration += delta;

	int i = (int)(currentDuration / frameDuration);
	currentDuration -= frameDuration * i;
	currentFrame = (currentFrame + i) % sequence.size();
}

Utils::SpriteBox Animation::GetCurrentSpriteBox()
{
	return sequence[currentFrame];
}

const Hitbox& Animation::GetHitbox()
{
	return hitbox;
}

void Animation::Render(Utils::Vector2<float> position)
{
	Utils::SpriteBox sb = sequence[currentFrame];
	Utils::Vector2<int> cp = Game::GetActiveScene()->GetCameraPosition().Rounded();
	D3DXVECTOR3 p(round(position.x + sb.offset.x) - cp.x, round(position.y + sb.offset.y) - cp.y, 0);

	Game::GetD3DXSprite()->Draw(texture, &sb.rect, NULL, &p, D3DCOLOR_XRGB(255, 255, 255));
}

