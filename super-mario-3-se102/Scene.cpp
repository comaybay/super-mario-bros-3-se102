#include "Scene.h"
#include "Constants.h"
#include "Game.h"
#include "Entities.h"
#include "EntityManager.h"
#include "TextureManager.h"
#include "Group.h"
using namespace Utils;

Scene::Scene() :
	entityManager(nullptr),
	encodedWorld(nullptr),
	camera(Camera()),
	isInTransitionPause(false),
	renderMovablesInSPGridEnabled(true),
	backgroundColor(D3DCOLOR_XRGB(255, 255, 255))
{
	Game::EnableCollisionEngine(true);
}

Scene::~Scene()
{
	delete encodedWorld;
	delete entityManager;
	for (auto& pair : outOfWorldEventByLPEntity)
		delete pair.second;
}

void Scene::_Init(const Dimension<int>& worldTileDim, const D3DCOLOR& backgroundColor, LPEncodedWorld encodedWorld,
	LPEntityManager entityManager, const std::string& prevScenePath)
{
	this->worldTileDim = worldTileDim;
	this->backgroundColor = backgroundColor;
	this->encodedWorld = encodedWorld;
	this->entityManager = entityManager;
	this->prevScenePath = prevScenePath;
}

#include "ContentFactory.h"
void Scene::_Ready()
{
	//TODO: Remove test code
	if (!GetEntitiesOfGroup(Group::PLAYERS).empty()) {
		Vector2<float> pos = { 200, 320 };
		Entities::LPMario mario = static_cast<Entities::LPMario>(GetEntityOfGroup(Group::PLAYERS));
		mario->SetPosition(pos);
		//AddEntity(ContentFactory(mario).Create("Mushroom", { pos.x, pos.y }));
	}

	camera._SetParentScene(this);
	camera.OnReady();

	entityManager->ForEach([](LPEntity entity) { entity->OnReady(); });
	if (!IsEntityGroupEmpty(Group::PLAYERS))
		camera.FollowEntity(GetEntityOfGroup(Group::PLAYERS));
}

void Scene::Update(float delta)
{
	for (LPEntity newEntity : newEntitiesWaitList) {
		entityManager->Add(newEntity);
		newEntity->OnReady();
	}

	newEntitiesWaitList.clear();

	DetectAndNotifyOutOfWorld();

	auto updateEntity = [delta](LPEntity entity) {
		if (entity->_IsActive()) {
			entity->Update(delta);
			entity->PostUpdate();
		}
	};

	CellRange range = GetCellRangeAroundCamera();
	entityManager->GetGrid(GridType::STATIC_ENTITIES)->ForEachEntityIn(range, updateEntity);

	//Only update player if in transition pause
	if (isInTransitionPause && !IsEntityGroupEmpty(Group::NOT_AFFECTED_BY_TRANSITION_PAUSE)) {
		for (LPEntity entity : GetEntitiesOfGroup(Group::NOT_AFFECTED_BY_TRANSITION_PAUSE))
			updateEntity(entity);
	}
	else {

		LPDynamicGrid movableEntitiesSPGrid = static_cast<LPDynamicGrid>(entityManager->GetGrid(GridType::MOVABLE_ENTITIES));
		movableEntitiesSPGrid->ForEachEntityIn(range, updateEntity);
		movableEntitiesSPGrid->UpdateCells(range);
		for (LPEntity entity : entityManager->GetNonGridEntities())
			updateEntity(entity);

		camera.Update(delta);
	}

	entityManager->FreeEntitiesInQueue();
}

void Scene::OnEntityDestroy(LPEntity entity)
{
	outOfWorldEventByLPEntity.erase(entity);
}

void Scene::DetectAndNotifyOutOfWorld()
{
	for (LPEntity entity : outOutWorldUnsubscribeWaitList) {
		delete outOfWorldEventByLPEntity[entity];
		outOfWorldEventByLPEntity.erase(entity);
	}

	std::list<LPEvent<>> notifyList;

	for (auto& pair : outOfWorldEventByLPEntity)
		//avoid deletion during iteration
		if (IsOutOfWorld(pair.first))
			notifyList.push_back(pair.second);

	for (LPEvent<> event : notifyList)
		event->Notify();
}

bool Scene::IsOutOfWorld(LPEntity entity)
{
	Vector2<float> pos = entity->GetPosition();
	Dimension<float> dim = entity->GetCurrentSpriteDimension();

	return (
		pos.x + dim.width < 0 ||
		pos.x > worldTileDim.width * Constants::TILE_SIZE ||
		pos.y < 0 ||
		pos.y > worldTileDim.height * Constants::TILE_SIZE
		);
}

CellRange Scene::GetCellRangeAroundCamera() {
	Vector2<float> camPos = GetCameraPosition();
	Dimension<int> gameDim = Game::GetGameSettings().gameDimension;

	//add margin
	int marginSize = gameDim.width / 4;
	Vector2<float> pos = camPos - Vector2<int>(marginSize, marginSize);
	Dimension<int> dim = gameDim + Dimension<int>(marginSize, marginSize);

	return entityManager->GetGrid(GridType::STATIC_ENTITIES)->GetCellRangeFromRectangle(pos, dim);
}

void Scene::Render()
{
	std::list<LPEntity> entitiesRenderedBeforeWorld;
	std::list<LPEntity> entitiesRenderedrAfterWorld;
	GetRenderEntities(entitiesRenderedBeforeWorld, entitiesRenderedrAfterWorld);

	if (Game::BeginRender() == S_OK)
	{
		// Clear the whole window with background color
		Game::ColorFill(backgroundColor);

		for (LPEntity entity : entitiesRenderedBeforeWorld)
			if (entity->_IsActive())
				entity->Render();

		RenderWorld(&EncodedWorld::GetBackgroundIndex);
		RenderWorld(&EncodedWorld::GetForegroundIndex);

		for (LPEntity entity : entitiesRenderedrAfterWorld)
			if (entity->_IsActive())
				entity->Render();

		Game::EndRender();
	}

	Game::Present();
}

void Scene::GetRenderEntities(std::list<LPEntity>& entitiesRenderedBeforeWorld, std::list<LPEntity>& entitiesRenderedrAfterWorld) {
	auto addByRenderOrder = [&entitiesRenderedBeforeWorld, &entitiesRenderedrAfterWorld](LPEntity entity) {
		if (entity->_IsRenderedBeforeWorld())
			entitiesRenderedBeforeWorld.push_back(entity);
		else
			entitiesRenderedrAfterWorld.push_back(entity);
	};

	CellRange range = GetCellRangeAroundCamera();
	entityManager->GetGrid(GridType::STATIC_ENTITIES)->ForEachEntityIn(range, addByRenderOrder);

	if (renderMovablesInSPGridEnabled)
		entityManager->GetGrid(GridType::MOVABLE_ENTITIES)->ForEachEntityIn(range, addByRenderOrder);

	for (LPEntity entity : entityManager->GetNonGridEntities())
		addByRenderOrder(entity);
}

void Scene::RenderWorld(int(EncodedWorld::* getIndex)(int, int))
{
	Utils::Vector2<float> cp = camera.GetPosition().Rounded();
	Utils::Vector2<int> tileOffset = cp / Constants::TILE_SIZE;

	Utils::Dimension<int> gameDim = Game::GetGameSettings().gameDimension;

	//tiles need for rendering, +1 for seemless display when camera move
	Utils::Dimension<int> tileRange(
		gameDim.width / Constants::TILE_SIZE + 1,
		gameDim.height / Constants::TILE_SIZE + 1
	);

	//only render inside camera 
	//rendering start at from camera position (top left of game screen)
	//ingder tiles from left to rigtion (top left of game screen)
	//render tiles rom left to right, top to bottom 
	for (int y = tileOffset.y; y < tileOffset.y + tileRange.height; y++) {
		for (int x = tileOffset.x; x < tileOffset.x + tileRange.width; x++) {
			int index = (encodedWorld->*getIndex)(x, y);

			//0xFFF means no tile
			if (index != 0xFFF) {
				LPDIRECT3DTEXTURE9 texure = TextureManager::Get(encodedWorld->GetTextureId());
				RECT rect = GetTileBoundingBox(index);
				Vector2<float> renderPos((float)x * Constants::TILE_SIZE, (float)y * Constants::TILE_SIZE);
				Game::Draw(texure, rect, renderPos);
			}
		}
	}
}

void Scene::AddEntity(LPEntity entity)
{
	newEntitiesWaitList.insert(entity);
	entity->_SetParentScene(this);
}

void Scene::QueueFree(LPEntity entity)
{
	entityManager->QueueFree(entity);
}

bool Scene::IsEntityGroupEmpty(const std::string& groupName)
{
	return entityManager->GetEntitiesOfGroup(groupName).size() == 0;
}

const LPEntity Scene::GetEntityOfGroup(const std::string& groupName) {
	return *(entityManager->GetEntitiesOfGroup(groupName).begin());
}

const std::unordered_set<LPEntity>& Scene::GetEntitiesOfGroup(const std::string& groupName)
{
	return entityManager->GetEntitiesOfGroup(groupName);
}

void Scene::TransitionPause(bool state)
{
	isInTransitionPause = state;
	Game::EnableCollisionEngine(!state);
}

Dimension<int> Scene::GetWorldDimension()
{
	return Dimension<int>(worldTileDim.width * Constants::TILE_SIZE, worldTileDim.height * Constants::TILE_SIZE);
}

const Vector2<float>& Scene::GetCameraPosition()
{
	return camera.GetPosition();
}

Camera& Scene::GetCamera() {
	return camera;
}


const std::string& Scene::GetPrevScenePath()
{
	return prevScenePath;
}

RECT Scene::GetTileBoundingBox(int id)
{
	Dimension<int> texDim = TextureManager::GetDimensionOf(encodedWorld->GetTextureId());
	int numOfCols = texDim.width / (Constants::TILE_SIZE + 1); //+1 for space between tiles
	int row = id / numOfCols;
	int col = id % numOfCols;

	int left = 1 + (Constants::TILE_SIZE + 1) * col; //+1 for space between tiles
	int top = 1 + (Constants::TILE_SIZE + 1) * row; //+1 for space between tiles
	int right = left + Constants::TILE_SIZE;
	int bottom = top + Constants::TILE_SIZE;
	return RECT{ left, top, right, bottom };
}

