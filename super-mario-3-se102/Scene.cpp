#include "Scene.h"
#include "Game.h"
#include "Entities.h"
#include "EntityManager.h"
#include "TextureManager.h"
#include "Groups.h"

Scene::~Scene()
{
	delete encodedWorld;
}

Scene::Scene()
{
}

void Scene::_Init(Utils::Dimension worldTileDim, D3DCOLOR backgroundColor, LPEncodedWorld encodedWorld, LPEntityManager entityManager)
{
	this->worldTileDim = worldTileDim;
	this->backgroundColor = backgroundColor;
	this->encodedWorld = encodedWorld;
	this->entityManager = entityManager;
}
//TODO: REMOVE TEST CODE
//D3DCOLOR c = D3DCOLOR_XRGB(rand() / 300, rand() / 300, rand() / 300);
int i = 0;
LPEntityManager Scene::_GetEntityManager()
{
	return entityManager;
}
void Scene::Update(float delta)
{
	/*i++;
	if (i == 60) {
		EntityManager::QueueFree(EntityManager::GetGroup(Groups::PLAYER).front());
	}*/

	//if (EntityManager::GetGroup("Player").size() != 0) {
	//	int i = EntityManager::GetGroup("Player").size();
	//	CollisionData d = CollisionEngine::Detect(EntityManager::GetGroup("Player").front(), EntityManager::GetGroup("Goombas").front(), delta);
	//	CollisionData dh = CollisionEngine::Detect(EntityManager::GetGroup("Player").front(), EntityManager::GetGroup("CollisionBlocks").front(), delta);
	//	auto pBlock = EntityManager::GetGroup("CollisionBlocks").begin();
	//	std::advance(pBlock, 1);
	//	CollisionEngine::Detect(EntityManager::GetGroup("Player").front(), *pBlock, delta);
	//}
	CollisionEngine::Update(delta);
	entityManager->UpdateEntities(delta);
	entityManager->PostUpdateEntities();
	camera.Update();
}

void Scene::Render()
{
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


		RenderWorld(&EncodedWorld::GetBackgroundIndex);
		entityManager->RenderEntities();
		RenderWorld(&EncodedWorld::GetForegroundIndex);

		d3dxSprite->End();
		d3ddv->EndScene();
	}

	// Display back buffer content to the screen
	d3ddv->Present(NULL, NULL, NULL, NULL);
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
	entityManager->SetUpdateEntitiesInMovableSPGrid(false);
}

void Scene::_Ready()
{
	//TODO: REMOVE DEBUG CODE
	//LPEntity mario = new Entities::Mario(Utils::Vector2<float>(16 * 4, worldTileDim.height * 16 - 16 * 4));
	//EntityManager::AddToGroup(Groups::PLAYER, mario);
	//LPEntity goomba = new Entities::Goomba(Utils::Vector2<float>(16 * 5, worldTileDim.height * 16 - 16 * 4));
	//EntityManager::AddToGroup("Goombas", goomba);
	LPEntity ground = new Entities::CollisionWallType1(Utils::Vector2<float>(16 * 12, worldTileDim.height * 16 - 16 * 2), Utils::Dimension(16, 16));
	entityManager->AddToGroups({ Groups::COLLISION_WALLS, Groups::COLLISION_WALLS_TYPE_1 }, ground);

	entityManager->ForEach([](LPEntity entity) {
		entity->OnReady();
		});
	camera.FollowEntity(entityManager->GetEntitiesByGroup(Groups::PLAYER).front());
}

Utils::Dimension Scene::GetWorldDimension()
{
	int scale = Game::GetScale();
	return Utils::Dimension(worldTileDim.width * Game::TILE_SIZE, worldTileDim.height * Game::TILE_SIZE);
}

Utils::Vector2<float> Scene::GetCameraPosition()
{
	return camera.GetPosition();
}

RECT Scene::GetTileBoundingBox(int id)
{
	//tile map taken from https://www.spriters-resource.com/nes/supermariobros3/sheet/81122 with modifications
	//tile map consists of 73 columns and 21 rows
	int row = id / 73;
	int col = id % 73;

	int left = 1 + (Game::TILE_SIZE + 1) * col; //+1 for space between tiles
	int top = 1 + (Game::TILE_SIZE + 1) * row; //+1 for space between tiles
	int right = left + Game::TILE_SIZE;
	int bottom = top + Game::TILE_SIZE;
	return RECT{ left, top, right, bottom };
}

void Scene::RenderWorld(int(EncodedWorld::* getIndex)(int, int))
{
	Utils::Vector2<float> cp = camera.GetPosition();
	Utils::Vector2<float> tileOffset((int)cp.x / Game::TILE_SIZE, (int)cp.y / Game::TILE_SIZE);

	Utils::Dimension gameDim = Game::GetGameDimension();

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

			//FFF = 4095, no tile
			if (index != 4095) {
				D3DXVECTOR3 p(round(x * Game::TILE_SIZE - cp.x), round(y * Game::TILE_SIZE - cp.y), 0);
				RECT rect = GetTileBoundingBox(index);
				Game::GetD3DXSprite()->Draw(TextureManager::Get(TextureID::TILES), &rect, NULL, &p, D3DCOLOR_XRGB(255, 255, 255));
			}
		}
	}
}