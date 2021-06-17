#pragma once
#include "Hitbox.h"
#include <string>
#include <unordered_map>

class HitboxManager
{
public:
	static void Add(const std::string& id, LPHitbox hitbox);
	static LPConstHitbox Get(const std::string& id);
private:
	static std::unordered_map<std::string, LPHitbox> hitboxById;
	static LPHitbox TILE_SIZE_HITBOX;
	static LPHitbox NO_HITBOX;
};

