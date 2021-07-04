#pragma once
#include <unordered_map>
#include "Scene.h"
#include "Entity.h"
#include "EntityManager.h"
#include "EncodedWorld.h"
#include "Grid.h"
#include "DynamicGrid.h"

class SceneLoader
{
public:
	static LPScene LoadScene(std::string scenePath);
private:
	static std::string ParseWorldProperties(std::ifstream& file, std::string& sceneType, Utils::Dimension<float>& dim, D3DCOLOR& bgColor, std::string& prevScenePath);
	static std::string ParseEncodedWorld(std::ifstream& file, int world_width, const std::string& sceneType, LPEncodedWorld& encodedWorld);
	static std::string ParseSpatialPartitionGrid
	(std::ifstream& file, LPGrid& wallEntitySpatialGrid, LPGrid& staticEntitySpatialGrid, LPDynamicGrid& movableEntitySpatialGrid);
	static std::string ParseAndAddWallsEntities(std::ifstream& file, LPEntityManager entityManager, LPGrid wallEntitySpatialGrid);
	static std::string ParseAndAddOtherEntities
	(std::ifstream& file, LPEntityManager entityManager, LPGrid staticEntitySpatialGrid, LPDynamicGrid movableEntitySpatialGrid);
	static std::string ParseWorldMapNodes(std::ifstream& file, LPEntityManager entityManager);

	typedef LPEntity(*ParseEntityMethod)(const std::vector<std::string>& tokens);
	static std::unordered_map <std::string, ParseEntityMethod> parseMethodByEntityName;
	static LPEntity ParseMario(const std::vector<std::string>& tokens);
	static LPEntity ParseGoomba(const std::vector<std::string>& tokens);
	static LPEntity ParseParaGoomba(const std::vector<std::string>& tokens);
	static LPEntity ParseKoopa(const std::vector<std::string>& tokens);
	static LPEntity ParseParaKoopa(const std::vector<std::string>& tokens);
	static LPEntity ParseCoin(const std::vector<std::string>& tokens);
	static LPEntity ParseQuestionBlock(const std::vector<std::string>& tokens);
	static LPEntity ParseBrick(const std::vector<std::string>& tokens);

	static LPEntity ParseWMBush(const std::vector<std::string>& tokens);
	static LPEntity ParseWMHelpBubble(const std::vector<std::string>& tokens);
};

