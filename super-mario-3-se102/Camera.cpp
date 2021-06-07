#include "Camera.h"
#include "Game.h"
#include "Entity.h"
#include "Event.h"

using namespace Utils;
Camera::Camera()
{
	target = nullptr;
	position = Vector2(0, 0);
	onTargetDestroy = nullptr;
}

Vector2 Camera::GetPosition() const
{
	return position;
}

void Camera::Update()
{
	//TODO: implement a more complex version
	if (target == nullptr)
		return;

	Dimension worldDim = Game::GetScene()->GetWorldDimension();
	Dimension gameDim = Game::GetGameDimension();
	Dimension targetDim = target->GetDimension();
	Vector2 targetPos = target->GetPosition();

	//center around entity
	Vector2 newPosition(
		targetPos.x + targetDim.width / 2 - gameDim.width / 2,
		targetPos.y + targetDim.height / 2 - gameDim.height / 2
	);

	newPosition.x = min(max(newPosition.x, 0), worldDim.width - gameDim.width);
	newPosition.y = min(max(newPosition.y, 0), worldDim.height - gameDim.height);

	position = newPosition;
}

void Camera::FollowEntity(LPEntity entity)
{
	target = entity;
	entity->GetDestroyEvent()->Subscribe(this, &Camera::OnEntityDestroy);
}

void Camera::OnEntityDestroy(LPEntity _)
{
	target = nullptr;
}
