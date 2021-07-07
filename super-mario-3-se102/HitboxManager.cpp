#include "HitboxManager.h"
#include "Utils.h"
#include "Constants.h"
#include "Contains.h"

std::unordered_map<std::string, LPHitbox> HitboxManager::hitboxById;

LPHitbox HitboxManager::TILE_SIZE_HITBOX = new Hitbox(
	Utils::Vector2<float>(0, 0),
	Utils::Dimension<int>(Constants::TILE_SIZE, Constants::TILE_SIZE)
);

LPHitbox HitboxManager::NO_HITBOX = new Hitbox(
	Utils::Vector2<float>(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()),
	Utils::Dimension<int>(0, 0)
);

void HitboxManager::Add(const std::string& id, LPHitbox hitbox)
{
	hitboxById[id] = hitbox;
}

LPConstHitbox HitboxManager::Get(const std::string& id)
{
	if (id == HitboxId::TILE_SIZE_HITBOX)
		return TILE_SIZE_HITBOX;
	else if (id == HitboxId::NONE)
		return NO_HITBOX;

	if (!Utils::Contains(id, hitboxById)) {
		std::string msg = "Hitbox \"" + id + "\" not found";
		throw std::exception(msg.c_str());
	}

	return hitboxById[id];
}