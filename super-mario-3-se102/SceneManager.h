#pragma once
#include <map>
#include "Scene.h"
#include "Entity.h"
#include "EntityManager.h"
#include "EncodedWorld.h"
#include "Grid.h"
#include "DynamicGrid.h"

class SceneManager
{
public:
	static void AddScenePath(std::string scenePath, std::string id);
	static LPScene LoadWorld(std::string id);
private:
	static std::map<std::string, std::string> scenePathById;
	static std::string ParseWorldProperties(std::ifstream& file, Utils::Dimension& dim, D3DCOLOR& bgColor);
	static std::string ParseEncodedWorld(std::ifstream& file, int world_width, LPEncodedWorld& encodedWorld);
	static std::string ParseSpatialPartitionGrid
	(std::ifstream& file, LPGrid& wallEntitySpatialGrid, LPGrid& staticEntitySpatialGrid, LPDynamicGrid& movableEntitySpatialGrid);
	static std::string ParseAndAddWallsEntities(std::ifstream& file, LPEntityManager entityManager, LPGrid wallEntitySpatialGrid);
	static std::string ParseAndAddOtherEntities
	(std::ifstream& file, LPEntityManager entityManager, LPGrid staticEntitySpatialGrid, LPDynamicGrid movableEntitySpatialGrid);

	typedef LPEntity(*ParseEntityMethod)(const std::vector<std::string>& tokens);
	static std::map <std::string, ParseEntityMethod> parseMethodByEntityName;
	static LPEntity ParseMario(const std::vector<std::string>& tokens);
	static LPEntity ParseGoomba(const std::vector<std::string>& tokens);
	static LPEntity ParseParaGoomba(const std::vector<std::string>& tokens);
	static LPEntity ParseKoopa(const std::vector<std::string>& tokens);
	static LPEntity ParseParaKoopa(const std::vector<std::string>& tokens);
	static LPEntity ParseQuestionBlock(const std::vector<std::string>& tokens);
	static LPEntity ParseCoin(const std::vector<std::string>& tokens);
};

