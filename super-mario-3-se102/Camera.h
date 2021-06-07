#pragma once
#include "EntityManager.h"
#include "EventHandler.h"

class Camera
{
public:
	Camera();
	Utils::Vector2 GetPosition() const;
	void Update();
	void FollowEntity(LPEntity entity);
private:
	void OnEntityDestroy(LPEntity _);
	LPEntity target;
	Utils::Vector2 position;
	LPEventHandler<Camera, LPEntity> onTargetDestroy;
};

