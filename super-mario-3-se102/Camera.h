#pragma once
#include "EventHandler.h"
#include "Utils.h"

class Camera
{
public:
	Camera();
	Utils::Vector2<float> GetPosition() const;
	void Update();
	void FollowEntity(LPEntity entity);
private:
	void OnEntityDestroy(LPEntity _);
	LPEntity target;
	Utils::Vector2<float> position;
	LPEventHandler<Camera, LPEntity> onTargetDestroy;
};

