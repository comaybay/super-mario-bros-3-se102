#include "SceneLoader.h"
#include "EncodedWorld.h"
#include "EntityManager.h"
#include "Utils.h"
#include "ProcessingUtils.h"
#include "Contains.h"
#include "Entities.h"
#include "Game.h"

#include <fstream>
#include "TextureManager.h"

using namespace Utils;
using namespace ProcessingUtils;

std::unordered_map <std::string, SceneLoader::ParseEntityMethod> SceneLoader::parseMethodByEntityName =
{
	{"Mario", &SceneLoader::ParseMario},
	{"Goomba", &SceneLoader::ParseGoomba},
	{"ParaGoomba",&SceneLoader::ParseParaGoomba},
	{"Koopa",&SceneLoader::ParseKoopa},
	{"ParaKoopa", &SceneLoader::ParseParaKoopa},
	{"Coin", &SceneLoader::ParseCoin},
	{"QuestionBlock", &SceneLoader::ParseQuestionBlock},
	{"Brick", &SceneLoader::ParseBrick},
};

LPScene SceneLoader::LoadScene(std::string scenePath) {
	std::ifstream file(scenePath);
	if (!file.is_open()) {
		std::string msg = "LoadWorld Failed: file not found: " + scenePath;
		throw std::exception(msg.c_str());
	}

	std::string section;
	std::string sceneType;
	Dimension worldDim;
	D3DCOLOR bgColor{};
	char* background;
	char* foreground;
	LPEntityManager entityManager = nullptr;
	LPEncodedWorld encodedWorld = nullptr;
	//TODO: Parse Grid dimension 
	LPGrid wallEntitySpatialGrid = nullptr;
	LPGrid staticEntitySpatialGrid = nullptr;
	LPDynamicGrid movableEntitySpatialGrid = nullptr;
	LPScene scene = new Scene();
	while (std::getline(file, section)) {
		if (section[0] != '[')
			continue;

		//TODO: Replace geline with GetNextNonCommentLine
		while (section[0] == '[') {
			if (section == "[WORLD PROPERTIES]")
				section = ParseWorldProperties(file, sceneType, worldDim, bgColor);
			else if (section == "[ENCODED WORLD]")
				section = ParseEncodedWorld(file, worldDim.width, sceneType, encodedWorld);
			else if (section == "[SPATIAL PARTITION GRID]") {
				section = ParseSpatialPartitionGrid(file, wallEntitySpatialGrid, staticEntitySpatialGrid, movableEntitySpatialGrid);
				entityManager = new EntityManager(scene, wallEntitySpatialGrid, staticEntitySpatialGrid, movableEntitySpatialGrid);
			}
			else if (section == "[WALL ENTITIES]")
				section = ParseAndAddWallsEntities(file, entityManager, wallEntitySpatialGrid);
			else if (section == "[WORLD ENTITIES]")
				section = ParseAndAddOtherEntities(file, entityManager, staticEntitySpatialGrid, movableEntitySpatialGrid);
			else if (section == "[WORLD MAP NODES]")
				section = ParseWorldMapNodes(file, entityManager);
			else
				break;
		}
	}
	file.close();

	scene->_Init(worldDim, bgColor, encodedWorld, entityManager);
	scene->_Ready();
	return scene;
}

std::string SceneLoader::ParseWorldProperties(std::ifstream& file, std::string& sceneType, Dimension& dim, D3DCOLOR& bgColor)
{
	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '#' || line == "")
			continue;

		std::vector<std::string> sceneTypeToken = SplitByComma(line);
		if (sceneTypeToken.size() != 1)
			throw InvalidTokenSizeException(1);
		line = GetNextNonCommentLine(file);

		std::vector<std::string> dimTokens = SplitByComma(line);
		if (dimTokens.size() != 2)
			throw InvalidTokenSizeException(2);

		std::getline(file, line);
		std::vector<std::string> colorTokens = SplitByComma(line);
		if (colorTokens.size() != 3)
			throw InvalidTokenSizeException(3);

		sceneType = sceneTypeToken[0];
		dim = Dimension(stof(dimTokens[0]), stof(dimTokens[1]));
		bgColor = D3DCOLOR_XRGB(stoi(colorTokens[0]), stoi(colorTokens[1]), stoi(colorTokens[2]));

		return line;
	}
	return line;
}

std::string SceneLoader::ParseEncodedWorld(std::ifstream& file, int world_width, const std::string& sceneType, LPEncodedWorld& encodedWorld)
{
	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '#' || line == "")
			continue;

		int size = line.size() + 1; //+1 because strcpy_s require room for null terminate char
		char* background = new char[size];
		char* foreground = new char[size];

		strcpy_s(background, sizeof(char) * size, line.c_str());
		std::getline(file, line);
		strcpy_s(foreground, sizeof(char) * size, line.c_str());

		std::string textureId = (sceneType == "WorldMap") ? TextureId::WORLD_MAP_TILES : TextureId::WORLD_TILES;

		encodedWorld = new EncodedWorld(size, world_width * 3, background, foreground, textureId);
		std::getline(file, line);

		return line;
	}
	throw std::exception("Error parsing encoded world");
}

std::string SceneLoader::ParseSpatialPartitionGrid
(std::ifstream& file, LPGrid& wallEntitySpatialGrid, LPGrid& staticEntitySpatialGrid, LPDynamicGrid& movableEntitySpatialGrid)
{
	std::string line;

	line = GetNextNonCommentLine(file);
	std::vector<std::string> cellDimTokens = SplitByComma(line);
	if (cellDimTokens.size() != 2)
		throw InvalidTokenSizeException(2);

	line = GetNextNonCommentLine(file);
	std::vector<std::string> gridSizeTokens = SplitByComma(line);
	if (gridSizeTokens.size() != 2)
		throw InvalidTokenSizeException(2);

	Dimension cellDim(stoi(cellDimTokens[0]), stoi(cellDimTokens[1]));
	int numOfCols = stoi(gridSizeTokens[0]);
	int numOfRows = stoi(gridSizeTokens[1]);

	wallEntitySpatialGrid = new Grid(numOfCols, numOfRows, cellDim);
	staticEntitySpatialGrid = new Grid(numOfCols, numOfRows, cellDim);
	movableEntitySpatialGrid = new DynamicGrid(numOfCols, numOfRows, cellDim);

	return line;
}

std::string SceneLoader::ParseAndAddWallsEntities(std::ifstream& file, LPEntityManager entityManager, LPGrid wallEntitySpatialGrid)
{
	//TODO:
	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '[' || line == "")
			return line;

		if (line[0] == '#' || line == "")
			continue;

		std::vector<std::string> entityTokens = SplitByComma(line);
		if (entityTokens.size() != 5)
			throw InvalidTokenSizeException(5);

		std::getline(file, line);
		std::vector<std::string> cellTokens = SplitByComma(line);
		if (cellTokens.size() != 4)
			throw InvalidTokenSizeException(4);

		Vector2<float> pos(stoi(entityTokens[1]), stoi(entityTokens[2]));
		Dimension dim(stoi(entityTokens[3]), stoi(entityTokens[4]));
		Vector2<int> cellIndex(stoi(cellTokens[0]), stoi(cellTokens[1]));
		Vector2<int> cellSpan(stoi(cellTokens[2]), stoi(cellTokens[3]));

		LPEntity entity;
		if (entityTokens[0] == "WallType1")
			entity = new Entities::CollisionWallType1(pos, dim);
		else if (entityTokens[0] == "WallType2")
			entity = new Entities::CollisionWallType2(pos, dim);
		else {
			std::string msg = "ParseAndAddWallsEntities failed: undefined wall type: " + entityTokens[0];
			throw std::exception(msg.c_str());
		}

		entityManager->_AddToWallSPGrid(entity, CellRange(cellIndex, cellSpan));
	}
	return line;
}

std::string SceneLoader::ParseAndAddOtherEntities
(std::ifstream& file, LPEntityManager entityManager, LPGrid staticEntitySpatialGrid, LPDynamicGrid movableEntitySpatialGrid)
{
	std::string line;
	while (std::getline(file, line)) {
		if (line[0] == '[' || line == "")
			return line;

		if (line[0] == '#' || line == "")
			continue;

		std::vector<std::string> entityTokens = SplitByComma(line);

		std::string name = entityTokens[0];
		//TODO: remove this line of code when everything is implemented
		if (!Contains(name, parseMethodByEntityName))
			continue;

		std::string isInAnyGrid = entityTokens[entityTokens.size() - 1];

		LPEntity entity = parseMethodByEntityName[name](entityTokens);

		if (isInAnyGrid == "False") {
			entityManager->_AddWithoutPutToGrid(entity);
			continue;
		}

		std::getline(file, line);
		std::vector<std::string> cellTokens = SplitByComma(line);
		Vector2<int> cellIndex(stoi(cellTokens[0]), stoi(cellTokens[1]));

		entityManager->_AddToNonWallSPGrid(entity, cellIndex);
	}

	return line;
}

std::string SceneLoader::ParseWorldMapNodes(std::ifstream& file, LPEntityManager entityManager)
{
	using namespace Entities;
	std::string line;
	while (true) {
		line = GetNextNonCommentLine(file);
		if (line[0] == '[' || line == "EOF")
			return line;

		std::vector<std::string> nodeIds = SplitByComma(line);
		std::unordered_map<std::string, LPWMNode> nodeById;
		for (auto& id : nodeIds)
			nodeById[id] = new WMNode();

		for (int i = 0; i < nodeIds.size(); i++) {
			line = GetNextNonCommentLine(file);
			std::vector<std::string> nodeTokens = SplitByComma(line);
			if (nodeTokens.size() != 10)
				throw InvalidTokenSizeException(10);

			std::string nodeId = nodeTokens[0];
			std::string scenePath = nodeTokens[1];
			LPWMNode topNode = nodeById[nodeTokens[2]];
			LPWMNode leftNode = nodeById[nodeTokens[3]];
			LPWMNode bottomNode = nodeById[nodeTokens[4]];
			LPWMNode rightNode = nodeById[nodeTokens[5]];
			Vector2<float> pos(stoi(nodeTokens[6]), stoi(nodeTokens[7]));
			Vector2<int> cellIndex(stoi(nodeTokens[8]), stoi(nodeTokens[9]));

			LPWMNode node = nodeById[nodeId];
			node->_Init(pos, scenePath, topNode, leftNode, bottomNode, rightNode);
			entityManager->_AddToNonWallSPGrid(node, cellIndex);
		}

		//test code
		LPWMNode startNode = nodeById["NStart"];
		startNode->Activate(new Mario(startNode->GetPosition()));

		return line;
	}
}

LPEntity SceneLoader::ParseMario(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 4)
		throw InvalidTokenSizeException(4);

	return new Entities::Mario(Vector2<float>(stoi(tokens[1]), stoi(tokens[2])));
}


LPEntity SceneLoader::ParseGoomba(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	return new Entities::Goomba(tokens[1], Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}

LPEntity SceneLoader::ParseParaGoomba(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	return new Entities::ParaGoomba(tokens[1], Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}

LPEntity SceneLoader::ParseKoopa(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	return new Entities::Koopa("Green", Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}

LPEntity SceneLoader::ParseParaKoopa(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	return new Entities::ParaKoopa("Green", Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}

LPEntity SceneLoader::ParseCoin(const std::vector<std::string>& tokens) {
	if (tokens.size() != 4)
		throw InvalidTokenSizeException(4);

	return new Entities::Coin(Vector2<float>(stoi(tokens[1]), stoi(tokens[2])));
}

LPEntity SceneLoader::ParseQuestionBlock(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	//TODO: REMOVE TEST CODE
	LPEntity content = new Entities::Goomba("Brown", Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));

	return new Entities::QuestionBlock(content, Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}

LPEntity SceneLoader::ParseBrick(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	return new Entities::Brick(nullptr, Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}
