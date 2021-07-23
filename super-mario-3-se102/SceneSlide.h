#pragma once
#include "Scene.h"

class SceneSlide : public Scene
{
public:
	void _Ready() override;
	void Update(float delta) override;

private:
	static const float SLIDE_SPEED;
};

