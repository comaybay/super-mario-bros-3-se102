#pragma once
#include "EventHandler.h"
#include "Utils.h"

class Scene;
typedef Scene* LPScene;

class Camera
{
public:
	Camera(LPScene parentScene);
	const Utils::Vector2<float>& GetPosition() const;
	void Update();
	void FollowEntity(LPEntity entity);
private:
	void OnEntityDestroy(LPEntity _);
	LPScene parentScene;
	LPEntity target;
	Utils::Vector2<float> position;
};

