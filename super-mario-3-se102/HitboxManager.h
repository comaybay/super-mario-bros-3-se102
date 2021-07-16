#pragma once
#include "Hitbox.h"
#include <string>
#include <unordered_map>

class HitboxManager
{
public:
	static void Add(const std::string& id, const Hitbox& hitbox);
	static const Hitbox& Get(const std::string& id);
private:
	static std::unordered_map<std::string, Hitbox> hitboxById;
	static Hitbox TILE_SIZE_HITBOX;
	static Hitbox NO_HITBOX;
};

