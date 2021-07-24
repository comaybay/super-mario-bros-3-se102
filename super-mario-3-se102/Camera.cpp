#include "Camera.h"
#include "Scene.h"
#include "Group.h"
#include "Game.h"
#include "Event.h"
#include "PlayerVariables.h"

using namespace Utils;

Camera::Camera(const Vector2<float>& position, const Utils::Dimension<int>& viewportDim)
	: Entity::Entity(position, { Group::CAMERAS , Group::NOT_AFFECTED_BY_TRANSITION_PAUSE }, GridType::NONE),
	viewportDim(viewportDim),
	target(nullptr),
	basePosY(0),
	followYOnly(false)
{
}

void Camera::OnReady() {
	Entity::OnReady();
	parentScene->UnsubscribeToOutOfWorldEvent(this);
	basePosY = parentScene->GetWorldDimension().height - viewportDim.height;
}

void Camera::FocusOn(LPEntity entity)
{
	Dimension<float> worldDim = parentScene->GetWorldDimension();
	Dimension<int> targetDim = entity->GetHitbox().GetDimension();
	const Vector2<float>& targetPos = entity->GetPosition();

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

void Camera::FocusXOn(LPEntity entity) {
	float worldDimX = static_cast<float>(parentScene->GetWorldDimension().width);
	int targetDimX = entity->GetHitbox().GetDimension().width;
	float targetPosX = entity->GetPosition().x;

	//center around target
	float newPosX = targetPosX + targetDimX / 2 - viewportDim.width / 2;

	newPosX += offset.x;

	//keep camera inside of world
	newPosX = Clip(newPosX, 0.0f, worldDimX - viewportDim.width);
	position.x = newPosX;
}

void Camera::FocusYOn(LPEntity entity) {
	float worldDimY = parentScene->GetWorldDimension().height;
	int targetDimY = entity->GetHitbox().GetDimension().height;
	float targetPosY = entity->GetPosition().y;

	//center around target
	float newPosY = targetPosY + targetDimY / 2 - viewportDim.height / 2;

	newPosY += offset.y;

	//keep camera inside of world
	newPosY = Clip(newPosY, 0.0f, worldDimY - viewportDim.height);
	position.y = newPosY;
}

void Camera::Update(float delta)
{
	Entity::Update(delta);

	if (target == nullptr)
		return;

	if (!followYOnly)
		FocusXOn(target);

	if (PlayerVariables::GetPowerMeterValue() == PlayerVariables::MAX_POWER_METER_VALUE ||
		position.y < basePosY ||
		PlayerVariables::IsInPipe())
		FocusYOn(target);
}

void Camera::FollowEntity(LPEntity entity, const Utils::Vector2<float>& offset)
{
	this->offset = offset;
	target = entity;
	entity->GetDestroyEvent().Subscribe(this, &Camera::OnTargetDestroy);
}

void Camera::FollowYEntity(LPEntity entity, const Utils::Vector2<float>& offset)
{
	followYOnly = true;
	FollowEntity(entity, offset);
}

const Utils::Dimension<int>& Camera::GetViewportDimension()
{
	return viewportDim;
}

void Camera::_SetVieportDimension(const Utils::Dimension<int>& viewportDimension)
{
	viewportDim = viewportDimension;
}

void Camera::StopFollowingEntity()
{
	target = nullptr;
	followYOnly = false;
}

void Camera::OnTargetDestroy(LPEntity entity)
{
	if (target == entity) {
		target = nullptr;
		followYOnly = false;
	}
}

