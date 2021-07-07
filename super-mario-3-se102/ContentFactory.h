#pragma once
#include "Entity.h"
#include "Mario.h"
#include <unordered_map>

/// <summary>
/// Factory class used for creating content for question blocks, bricks, hidden blocks, etc...
/// </summary>
class ContentFactory
{
public:
	ContentFactory(Entities::LPMario player);
	LPEntity Create(const std::string& contentId, const Utils::Vector2<float>& position);

private:
	Entities::LPMario player;
	LPEntity CreateCoin(const Utils::Vector2<float>& position);
	LPEntity CreateMushroom(const Utils::Vector2<float>& position);
	LPEntity CreateSuperLeaf(const Utils::Vector2<float>& position);

	typedef LPEntity(ContentFactory::* CreateItemMethod)(const Utils::Vector2<float>&);
	static const std::unordered_map<std::string, CreateItemMethod> createMethodByContentId;
};

struct ContentId {
public:
	static std::string NONE;
};
