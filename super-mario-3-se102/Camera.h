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
	Camera(const Utils::Vector2<float>& position = { 0, 0 }, const Utils::Dimension<int>& viewportDim = { 0, 0 });
	void Update(float _) override;
	void OnReady() override;
	void FocusOn(LPEntity entity);
	void FocusXOn(LPEntity entity);
	void FocusYOn(LPEntity entity);
	void FollowEntity(LPEntity entity, const Utils::Vector2<float>& offset = { 0, 0 });
	void FollowYEntity(LPEntity entity, const Utils::Vector2<float>& offset = { 0, 0 });
	const Utils::Dimension<int>& GetViewportDimension();
	void _SetVieportDimension(const Utils::Dimension<int>& viewportDimension);
	void StopFollowingEntity();
private:
	void OnTargetDestroy(LPEntity target);
	float basePosY;
	LPEntity target;
	LPEntity player;
	Utils::Dimension<int> viewportDim;
	Utils::Vector2<float> offset;
	bool followYOnly;
};
typedef Camera* LPCamera;

