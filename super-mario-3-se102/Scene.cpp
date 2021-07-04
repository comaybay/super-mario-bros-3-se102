#include "Scene.h"
#include "Game.h"
#include "Entities.h"
#include "EntityManager.h"
#include "TextureManager.h"
#include "Group.h"
using namespace Utils;

Scene::Scene() :
	camera(Camera(this)),
	entityManager(nullptr),
	updateMovablesInSPGridEnabled(true),
	renderMovablesInSPGridEnabled(true),
	backgroundColor(D3DCOLOR_XRGB(255, 255, 255))
{
	Game::EnableCollisionEngine(true);
}

Scene::~Scene()
{
	delete encodedWorld;
	delete entityManager;
}

void Scene::_Init(const Dimension& worldTileDim, const D3DCOLOR& backgroundColor, LPEncodedWorld encodedWorld,
	LPEntityManager entityManager, const std::string& prevScenePath)
{
	this->worldTileDim = worldTileDim;
	this->backgroundColor = backgroundColor;
	this->encodedWorld = encodedWorld;
	this->entityManager = entityManager;
	this->prevScenePath = prevScenePath;
}
//TODO: REMOVE TEST CODE
//D3DCOLOR c = D3DCOLOR_XRGB(rand() / 300, rand() / 300, rand() / 300);
int i = 0;
void Scene::Update(float delta)
{
	/*i++;
	if (i == 60) {
		EntityManager::QueueFree(EntityManager::GetGroup(Group::PLAYER).front());
	}*/

	//if (EntityManager::GetGroup("Player").size() != 0) {
	//	int i = EntityManager::GetGroup("Player").size();
	//	CollisionData d = CollisionEngine::Detect(EntityManager::GetGroup("Player").front(), EntityManager::GetGroup("Goombas").front(), delta);
	//	CollisionData dh = CollisionEngine::Detect(EntityManager::GetGroup("Player").front(), EntityManager::GetGroup("CollisionBlocks").front(), delta);
	//	auto pBlock = EntityManager::GetGroup("CollisionBlocks").begin();
	//	std::advance(pBlock, 1);
	//	CollisionEngine::Detect(EntityManager::GetGroup("Player").front(), *pBlock, delta);
	//}
	NotifyOutOfWorldEntities();
	CellRange range = GetCellRangeAroundCamera();

	auto forEachEntity = [this, range](std::function<void(LPEntity)> handler) {
		entityManager->GetGrid(GridType::STATIC_ENTITIES)->ForEachEntityIn(range, handler);

		if (updateMovablesInSPGridEnabled) {
			LPDynamicGrid movableEntitiesSPGrid = static_cast<LPDynamicGrid>(entityManager->GetGrid(GridType::MOVABLE_ENTITIES));
			movableEntitiesSPGrid->ForEachEntityIn(range, handler);
			movableEntitiesSPGrid->UpdateCells(range);
		}

		for (LPEntity entity : entityManager->GetNonGridEntities())
			handler(entity);
	};

	auto update = [delta](LPEntity entity) { entity->Update(delta); };
	auto postUpdate = [](LPEntity entity) { entity->PostUpdate(); };

	forEachEntity(update);
	camera.Update();
	forEachEntity(postUpdate);

	entityManager->FreeEntitiesInQueue();
}

void Scene::OnEntityDestroy(LPEntity entity)
{
	subscriptions.erase(entity);
}

void Scene::NotifyOutOfWorldEntities()
{
	notifyList.clear();

	for (auto& pair : subscriptions)
		//avoid deletion during iteration
		if (IsOutOfWorld(pair.first))
			notifyList.push_back(pair);

	for (auto& pair : notifyList)
		(pair.first->*pair.second)();
}

bool Scene::IsOutOfWorld(LPEntity entity)
{
	Vector2<float> pos = entity->GetPosition();
	Dimension dim = entity->GetCurrentSpriteDimension();

	return (
		pos.x + dim.width < 0 ||
		pos.x > worldTileDim.width * Game::TILE_SIZE ||
		pos.y < 0 ||
		pos.y > worldTileDim.height * Game::TILE_SIZE
		);
}

CellRange Scene::GetCellRangeAroundCamera() {
	Vector2<float> camPos = GetCameraPosition();
	Dimension dim = Game::GetGameSettings().gameDimension;

	//add margin
	int marginSize = dim.width / 4;
	camPos = camPos - Vector2<float>(marginSize, marginSize);
	dim.width += marginSize;
	dim.height += marginSize;

	return entityManager->GetGrid(GridType::STATIC_ENTITIES)->GetCellRangeFromRectangle(camPos, dim);
}

void Scene::Render()
{
	GetRenderEntities(entitiesRenderedBeforeWorld, entitiesRenderedrAfterWorld);

	LPDIRECT3DDEVICE9 d3ddv = Game::GetDirect3DDevice();
	if (d3ddv->BeginScene() == S_OK)
	{
		// Clear the whole window with a color
		d3ddv->ColorFill(Game::GetBackBuffer(), NULL, backgroundColor);

		LPD3DXSPRITE d3dxSprite = Game::GetD3DXSprite();
		d3dxSprite->Begin(D3DXSPRITE_ALPHABLEND);

		//nearest neightbor filter
		d3ddv->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		d3ddv->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		d3ddv->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

		for (LPEntity entity : entitiesRenderedBeforeWorld)
			entity->Render();

		RenderWorld(&EncodedWorld::GetBackgroundIndex);
		RenderWorld(&EncodedWorld::GetForegroundIndex);

		for (LPEntity entity : entitiesRenderedrAfterWorld)
			entity->Render();

		d3dxSprite->End();
		d3ddv->EndScene();
	}

	// Display back buffer content to the screen
	d3ddv->Present(NULL, NULL, NULL, NULL);
}


void Scene::GetRenderEntities(std::unordered_set<LPEntity>& entitiesRenderedBeforeWorld, std::unordered_set<LPEntity>& entitiesRenderedrAfterWorld) {
	auto addByRenderOrder = [&entitiesRenderedBeforeWorld, &entitiesRenderedrAfterWorld](LPEntity entity) {
		if (entity->_IsRenderedBeforeWorld())
			entitiesRenderedBeforeWorld.insert(entity);
		else
			entitiesRenderedrAfterWorld.insert(entity);
	};

	CellRange range = GetCellRangeAroundCamera();
	entitiesRenderedBeforeWorld.clear();
	entitiesRenderedrAfterWorld.clear();

	entityManager->GetGrid(GridType::STATIC_ENTITIES)->ForEachEntityIn(range, addByRenderOrder);

	if (renderMovablesInSPGridEnabled)
		entityManager->GetGrid(GridType::MOVABLE_ENTITIES)->ForEachEntityIn(range, addByRenderOrder);

	for (LPEntity entity : entityManager->GetNonGridEntities())
		addByRenderOrder(entity);
}

void Scene::AddEntity(LPEntity entity)
{
	entityManager->Add(entity);
	entity->OnReady();
}

void Scene::QueueFree(LPEntity entity)
{
	entityManager->QueueFree(entity);
}

const std::list<LPEntity>& Scene::GetEntitiesByGroup(std::string groupName)
{
	return entityManager->GetEntitiesByGroup(groupName);
}

void Scene::PlayerDeath()
{
	updateMovablesInSPGridEnabled = false;
	Game::EnableCollisionEngine(false);
}

#include "ParaGoomba.h"
void Scene::_Ready()
{
	//TODO: REMOVE DEBUG CODE
	//LPEntity mario = new Entities::Mario(Utils::Vector2<float>(16 * 4, worldTileDim.height * 16 - 16 * 4));
	//EntityManager::AddToGroup(Group::PLAYER, mario);
	//LPEntity goomba = new Entities::Goomba(Utils::Vector2<float>(16 * 5, worldTileDim.height * 16 - 16 * 4));
	//EntityManager::AddToGroup("Goombas", goomba);
	LPEntity ground = new Entities::CollisionWallType1(Utils::Vector2<float>(16 * 12, worldTileDim.height * 16 - 16 * 2), Utils::Dimension(16, 16));
	entityManager->AddToGroups({ Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 }, ground);
	entityManager->Add(new Entities::ParaGoomba("Brown", Utils::Vector2<float>(16 * 16, worldTileDim.height * 16 - 16 * 2)));
	if (!entityManager->GetEntitiesByGroup(Group::PLAYER).empty())
		entityManager->GetEntitiesByGroup(Group::PLAYER).front()->SetPosition({ 2100, 200 });

	entityManager->ForEach([](LPEntity entity) { entity->OnReady(); });
	std::list<LPEntity> playerGroup = entityManager->GetEntitiesByGroup(Group::PLAYER);
	if (!playerGroup.empty())
		camera.FollowEntity(playerGroup.front());
}

const Dimension& Scene::GetWorldDimension()
{
	return Utils::Dimension(worldTileDim.width * Game::TILE_SIZE, worldTileDim.height * Game::TILE_SIZE);
}

const Vector2<float>& Scene::GetCameraPosition()
{
	return camera.GetPosition();
}

const std::string& Scene::GetPrevScenePath()
{
	return prevScenePath;
}

RECT Scene::GetTileBoundingBox(int id)
{
	Dimension texDim = TextureManager::GetDimensionOf(encodedWorld->GetTextureId());
	int numOfCols = texDim.width / (Game::TILE_SIZE + 1); //+1 for space between tiles
	int row = id / numOfCols;
	int col = id % numOfCols;

	int left = 1 + (Game::TILE_SIZE + 1) * col; //+1 for space between tiles
	int top = 1 + (Game::TILE_SIZE + 1) * row; //+1 for space between tiles
	int right = left + Game::TILE_SIZE;
	int bottom = top + Game::TILE_SIZE;
	return RECT{ left, top, right, bottom };
}

void Scene::RenderWorld(int(EncodedWorld::* getIndex)(int, int))
{
	Utils::Vector2<int> cp = camera.GetPosition().Rounded();
	Utils::Vector2<float> tileOffset(cp.x / Game::TILE_SIZE, cp.y / Game::TILE_SIZE);

	Utils::Dimension gameDim = Game::GetGameSettings().gameDimension;

	//tiles need for rendering, +1 for seemless display when camera move
	Utils::Dimension tileRange(
		gameDim.width / Game::TILE_SIZE + 1,
		gameDim.height / Game::TILE_SIZE + 1
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
				Vector2<float> renderPos(x * Game::TILE_SIZE, y * Game::TILE_SIZE);
				Game::Draw(texure, rect, renderPos);
			}
		}
	}
}

