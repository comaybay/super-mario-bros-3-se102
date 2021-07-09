#include "Camera.h"
#include "Scene.h"
#include "Game.h"
#include "Entity.h"
#include "Event.h"

using namespace Utils;
Camera::Camera(LPScene parentScene) :
	parentScene(parentScene)
{
	target = nullptr;
	position = Vector2<float>(0, 0);
}

const Vector2<float>& Camera::GetPosition() const
{
	return position;
}

void Camera::Update()
{
	if (target == nullptr)
		return;

	Dimension<float> worldDim = parentScene->GetWorldDimension();
	Dimension<float> gameDim = Game::GetGameSettings().gameDimension;
	Dimension<int> targetDim = target->GetCurrentSpriteDimension();
	const Vector2<float>& targetPos = target->GetPosition();

	//center around entity
	Vector2<float> newPosition(
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
	entity->GetDestroyEvent().Subscribe(this, &Camera::OnEntityDestroy);
	Update();
}

void Camera::StopFollowingEntity()
{
	target = nullptr;
}

void Camera::OnEntityDestroy(LPEntity entity)
{
	if (target == entity)
		target = nullptr;
}
