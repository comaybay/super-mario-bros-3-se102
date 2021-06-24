#include "SceneManager.h"
#include "EncodedWorld.h"
#include "EntityManager.h"
#include "Utils.h"
#include "ProcessingUtils.h"
#include "Contains.h"
#include "Entities.h"
#include <fstream>

using namespace Utils;
using namespace ProcessingUtils;

std::unordered_map<std::string, std::string> SceneManager::scenePathById;
std::unordered_map <std::string, SceneManager::ParseEntityMethod> SceneManager::parseMethodByEntityName =
{
	{"Mario", &SceneManager::ParseMario},
	{"Goomba", &SceneManager::ParseGoomba},
	{"ParaGoomba",&SceneManager::ParseParaGoomba},
	{"Koopa",&SceneManager::ParseKoopa},
	{"ParaKoopa", &SceneManager::ParseParaKoopa},
	{"QuestionBlock", &SceneManager::ParseQuestionBlock},
	{"Coin", &SceneManager::ParseCoin},
};

void SceneManager::AddScenePath(std::string scenePath, std::string id) {
	scenePathById[id] = scenePath;
}

LPScene SceneManager::LoadWorld(std::string id) {
	std::string scenePath = scenePathById[id];

	std::ifstream file(scenePath);
	if (!file.is_open()) {
		std::string msg = "LoadWorld Failed: file not found: " + scenePath;
		throw std::exception(msg.c_str());
	}

	std::string section;
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
				section = ParseWorldProperties(file, worldDim, bgColor);
			else if (section == "[ENCODED WORLD]")
				section = ParseEncodedWorld(file, worldDim.width, encodedWorld);
			else if (section == "[SPATIAL PARTITION GRID]") {
				section = ParseSpatialPartitionGrid(file, wallEntitySpatialGrid, staticEntitySpatialGrid, movableEntitySpatialGrid);
				entityManager = new EntityManager(scene, wallEntitySpatialGrid, staticEntitySpatialGrid, movableEntitySpatialGrid);
			}
			else if (section == "[WALL ENTITIES]")
				section = ParseAndAddWallsEntities(file, entityManager, wallEntitySpatialGrid);
			else if (section == "[WORLD ENTITIES]")
				section = ParseAndAddOtherEntities(file, entityManager, staticEntitySpatialGrid, movableEntitySpatialGrid);
			else
				continue;
		}
	}
	file.close();

	scene->_Init(worldDim, bgColor, encodedWorld, entityManager);
	scene->_Ready();
	return scene;
}

std::string SceneManager::ParseWorldProperties(std::ifstream& file, Dimension& dim, D3DCOLOR& bgColor)
{
	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '#' || line == "")
			continue;

		std::vector<std::string> dimTokens = SplitByComma(line);
		if (dimTokens.size() != 2)
			throw InvalidTokenSizeException(2);

		std::getline(file, line);
		std::vector<std::string> colorTokens = SplitByComma(line);
		if (colorTokens.size() != 3)
			throw InvalidTokenSizeException(3);

		dim = Dimension(stof(dimTokens[0]), stof(dimTokens[1]));
		bgColor = D3DCOLOR_XRGB(stoi(colorTokens[0]), stoi(colorTokens[1]), stoi(colorTokens[2]));

		return line;
	}
	return line;
}

std::string SceneManager::ParseEncodedWorld(std::ifstream& file, int world_width, LPEncodedWorld& encodedWorld)
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

		encodedWorld = new EncodedWorld(size, world_width * 3, background, foreground);
		std::getline(file, line);

		return line;
	}
	throw std::exception("Error parsing encoded world");
}

std::string SceneManager::ParseSpatialPartitionGrid
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

std::string SceneManager::ParseAndAddWallsEntities(std::ifstream& file, LPEntityManager entityManager, LPGrid wallEntitySpatialGrid)
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

std::string SceneManager::ParseAndAddOtherEntities
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

		if (isInAnyGrid == "False") {
			LPEntity entity = parseMethodByEntityName[name](entityTokens);
			entityManager->_AddWithoutPutToGrid(entity);
			continue;
		}


		std::getline(file, line);
		std::vector<std::string> cellTokens = SplitByComma(line);
		Vector2<int> cellIndex(stoi(cellTokens[0]), stoi(cellTokens[1]));

		LPEntity entity = parseMethodByEntityName[name](entityTokens);
		entityManager->_AddToNonWallSPGrid(entity, cellIndex);
	}

	return line;
}

LPEntity SceneManager::ParseMario(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 4)
		throw InvalidTokenSizeException(4);

	return new Entities::Mario(Vector2<float>(stoi(tokens[1]), stoi(tokens[2])));
}


LPEntity SceneManager::ParseGoomba(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	return new Entities::Goomba(tokens[1], Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}

LPEntity SceneManager::ParseParaGoomba(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	return new Entities::ParaGoomba(tokens[1], Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}

LPEntity SceneManager::ParseKoopa(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	return new Entities::Koopa("Green", Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}

LPEntity SceneManager::ParseParaKoopa(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	return new Entities::ParaKoopa("Green", Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}

LPEntity SceneManager::ParseQuestionBlock(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 5)
		throw InvalidTokenSizeException(5);

	//TODO: REMOVE TEST CODE
	LPEntity content = new Entities::Goomba("Brown", Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));

	return new Entities::QuestionBlock(content, Vector2<float>(stoi(tokens[2]), stoi(tokens[3])));
}

LPEntity SceneManager::ParseCoin(const std::vector<std::string>& tokens) {
	if (tokens.size() != 4)
		throw InvalidTokenSizeException(4);

	return new Entities::Coin(Vector2<float>(stoi(tokens[1]), stoi(tokens[2])));
}