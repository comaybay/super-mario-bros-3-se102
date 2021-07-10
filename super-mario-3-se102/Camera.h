#pragma once
#include "Entity.h"

class Scene;
typedef Scene* LPScene;

/// <summary>
/// this Entity is used internally by Scene.
/// </summary>
class Camera : public Entity
{
public:
	Camera(const Utils::Vector2<float>& position = { 0, 0 });
	void Update(float _) override;
	void OnReady() override;
	void FollowEntity(LPEntity entity, const Utils::Vector2<float>& offset = { 0, 0 });
	void StopFollowingEntity();
private:
	void OnEntityDestroy(LPEntity _);
	LPEntity target;
	Utils::Dimension<int> viewportDim;
	Utils::Vector2<float> offset;
};

