#include "Camera.h"
#include "Scene.h"
#include "Group.h"
#include "Game.h"
#include "Event.h"

using namespace Utils;
Camera::Camera(const Vector2<float>& position)
	: Entity::Entity(position, Group::CAMERAS, GridType::NONE),
	viewportDim(Game::GetGameSettings().gameDimension),
	target(nullptr)
{
}

void Camera::OnReady() {
	Entity::OnReady();
	parentScene->UnsubscribeToOutOfWorldEvent(this);
}

void Camera::Update(float delta)
{
	Entity::Update(delta);

	if (target == nullptr)
		return;

	Dimension<float> worldDim = parentScene->GetWorldDimension();
	Dimension<int> targetDim = target->GetCurrentSpriteDimension();
	const Vector2<float>& targetPos = target->GetPosition();

	//center around target
	Vector2<float> newPosition(
		targetPos.x + targetDim.width / 2 - viewportDim.width / 2,
		targetPos.y + targetDim.height / 2 - viewportDim.height / 2
	);

	newPosition += offset;

	//keep camera inside of world
	newPosition.x = Clip(newPosition.x, 0.0f, worldDim.width - viewportDim.width);
	newPosition.y = Clip(newPosition.y, 0.0f, worldDim.height - viewportDim.height);

	position = newPosition;
}

void Camera::FollowEntity(LPEntity entity, const Utils::Vector2<float>& offset)
{
	this->offset = offset;
	target = entity;
	entity->GetDestroyEvent().Subscribe(this, &Camera::OnEntityDestroy);
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
