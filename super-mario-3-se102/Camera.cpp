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

void Camera::Update(float delta)
{
	Entity::Update(delta);

	if (target != nullptr) {
		FocusOn(target);
		return;
	}

	//handling slide
	const float WorldDimX = parentScene->GetWorldDimension().width;
	if (!AlmostEqual(velocity.x, 0) && position.x > WorldDimX - viewportDim.width) {
		velocity.x = 0;
		position.x = WorldDimX - viewportDim.width;
	}

	if (player == nullptr && !parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		player = parentScene->GetEntityOfGroup(Group::PLAYERS);
		player->GetDestroyEvent().Subscribe(this, &Camera::OnPlayerDestroy);
	}

	if (player) {
		float worldDimY = parentScene->GetWorldDimension().height;
		int targetDimY = player->GetHitbox().GetDimension().height;
		float targetPosY = player->GetPosition().y;
		float newPositionY = targetPosY + targetDimY / 2 - viewportDim.height / 2;
		position.y = Clip(newPositionY, 0.0f, worldDimY - viewportDim.height);
	}
}

void Camera::FollowEntity(LPEntity entity, const Utils::Vector2<float>& offset)
{
	this->offset = offset;
	target = entity;
	entity->GetDestroyEvent().Subscribe(this, &Camera::OnTargetDestroy);
}

void Camera::StopFollowingEntity()
{
	target = nullptr;
}

void Camera::OnTargetDestroy(LPEntity entity)
{
	if (target == entity)
		target = nullptr;
}

void Camera::OnPlayerDestroy(LPEntity _)
{
	player = nullptr;
}
