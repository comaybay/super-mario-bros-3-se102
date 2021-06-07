#include "CollisionHandling.h"
#include "Groups.h"

void CollisionHandling::Slide(LPEntity entity, const CollisionData& data)
{
	Utils::Vector2 velocity = entity->GetRemainingVelocity();
	Utils::Vector2 position = entity->GetPosition();

	//stop at collision
	position += (velocity * data.value * data.delta);
	entity->SetPosition(position);

	//calculate slide velocity used for next Update call
	float remainingtime = 1.0f - data.value;
	float dotprod = (velocity.x * data.edge.y + velocity.y * data.edge.x) * remainingtime;
	velocity.x = data.edge.y * dotprod;
	velocity.y = data.edge.x * dotprod;

	entity->SetRemainingVelocity(velocity);
}
