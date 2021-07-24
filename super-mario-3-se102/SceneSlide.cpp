#include "SceneSlide.h"
#include "Group.h"
#include "EntityManager.h"

const float SceneSlide::SLIDE_SPEED = 30;

void SceneSlide::_Ready()
{
	entityManager->ForEach([this](LPEntity entity) {
		entity->_SetParentScene(this);
		entity->OnReady();
		}
	);

	camera._SetParentScene(this);
	camera.OnReady();
	camera.SetVelocity({ SLIDE_SPEED, 0 });

	if (!IsEntityGroupEmpty(Group::PLAYERS)) {
		LPEntity player = GetEntityOfGroup(Group::PLAYERS);
		camera.FocusOn(player);
		camera.FollowYEntity(player);

		timer.Start();
	}
}

void SceneSlide::Update(float delta)
{
	Scene::Update(delta);

	//handling slide stop
	const float WorldDimX = GetWorldDimension().width;
	const Utils::Dimension<int>& viewportDim = camera.GetViewportDimension();
	if (camera.GetPosition().x > WorldDimX - viewportDim.width) {
		camera.SetVelocity({ 0, 0 });
		camera.SetPosition({ WorldDimX - viewportDim.width , camera.GetPosition().y });
	}
}
