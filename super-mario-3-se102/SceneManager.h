#pragma once
#include "Scene.h"
#include "Entity.h"
#include "EncodedWorld.h"
#include <map>



class SceneManager
{
public:
	static void AddScenePath(std::string scenePath, std::string id);
	static LPScene LoadWorld(std::string id);
private:
	static std::string ParseWorldProperties(std::ifstream& file, Utils::Dimension& dim, D3DCOLOR& bgColor);
	static std::string ParseEncodedWorld(std::ifstream& file, int world_width, LPEncodedWorld& encodedWorld);
	static std::string ParseAndAddWallsEntities(std::ifstream& file, std::vector<LPEntity>* entities);
	static std::string ParseAndAddOtherEntities(std::ifstream& file, std::vector<LPEntity>* entities);
	static std::map<std::string, std::string> scenePathById;

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

