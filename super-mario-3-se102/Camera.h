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
	void FocusOn(LPEntity entity);
	void FollowEntity(LPEntity entity, const Utils::Vector2<float>& offset = { 0, 0 });
	void StopFollowingEntity();
private:
	void OnTargetDestroy(LPEntity target);
	void OnPlayerDestroy(LPEntity _);
	LPEntity target;
	LPEntity player;
	Utils::Dimension<int> viewportDim;
	Utils::Vector2<float> offset;
};

