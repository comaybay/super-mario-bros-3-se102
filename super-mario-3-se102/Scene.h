#pragma once
#include <string>
#include <vector>
#include "Utils.h"
#include "EncodedWorld.h"
#include "Entity.h"
#include "Camera.h"

class Scene
{
public:
	//~Scene();
	Scene(Utils::Dimension worldTileDim, D3DCOLOR backgroundColor, LPEncodedWorld encodedWorld, std::vector<LPEntity>* entities);
	~Scene();
	void Update(float delta);
	void Render();
	Utils::Dimension GetWorldDimension();
	Utils::Vector2 GetCameraPosition();
private:
	void OnEntityDestroy(LPEntity entity);
	void RenderWorld(int(EncodedWorld::* getIndex)(int, int));
	RECT GetTileBoundingBox(int id);
	Utils::Dimension worldTileDim;
	LPEncodedWorld encodedWorld;
	D3DCOLOR backgroundColor;
	Camera camera;
	std::vector<LPEntity>* entities;
};
typedef Scene* LPScene;
