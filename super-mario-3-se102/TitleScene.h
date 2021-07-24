#pragma once
#include "Scene.h"

class TitleScene : public Scene {
public:
	TitleScene();;
	void _Ready() override;
	void Update(float delta) override;
	void Render() override;

	LPAnimation titleScreenAnim;;
};

