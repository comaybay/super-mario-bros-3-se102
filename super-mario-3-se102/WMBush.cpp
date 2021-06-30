#include "WMBush.h"

Entities::WMBush::WMBush(const Utils::Vector2<float>& position) :
	Entity::Entity(position, "WMBush", HitboxId::NONE, "WMBushes", GridType::STATIC_ENTITIES)
{
}
