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
	if (!IsEntityGroupEmpty(Group::PLAYERS))
		camera.FocusOn(GetEntityOfGroup(Group::PLAYERS));
}
