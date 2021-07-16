#include "HitboxManager.h"
#include "Utils.h"
#include "Constants.h"
#include "Contains.h"

std::unordered_map<std::string, Hitbox> HitboxManager::hitboxById;

Hitbox HitboxManager::TILE_SIZE_HITBOX = Hitbox(
	Utils::Vector2<float>(0, 0),
	Utils::Dimension<int>(Constants::TILE_SIZE, Constants::TILE_SIZE)
);

Hitbox HitboxManager::NO_HITBOX = Hitbox(
	Utils::Vector2<float>(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()),
	Utils::Dimension<int>(0, 0)
);

void HitboxManager::Add(const std::string& id, const Hitbox& hitbox)
{
	hitboxById[id] = hitbox;
}

const Hitbox& HitboxManager::Get(const std::string& id)
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