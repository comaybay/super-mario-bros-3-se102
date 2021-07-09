#include "WMMario.h"
#include "Group.h"

Entities::WMMario::WMMario(const Utils::Vector2<float>& position) :
	Entity::Entity(position, "WMMario", HitboxId::NONE, { Group::PLAYERS, "WMMario" }, GridType::NONE)
{
}
