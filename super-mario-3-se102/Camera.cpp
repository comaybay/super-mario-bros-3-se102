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
	onTargetDestroy = nullptr;
}

const Vector2<float>& Camera::GetPosition() const
{
	return position;
}

void Camera::Update()
{
	//TODO: implement a more complex version
	if (target == nullptr)
		return;

	Dimension worldDim = parentScene->GetWorldDimension();
	Dimension gameDim = Game::GetGameSettings().gameDimension;
	Dimension targetDim = target->GetCurrentSpriteDimension();
	Vector2<float> targetPos = target->GetPosition();

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

void Camera::OnEntityDestroy(LPEntity _)
{
	target = nullptr;
}
