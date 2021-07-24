#include "ContentFactory.h"
#include "Contains.h"
#include "CoinUp.h"
#include "Mushroom.h"
#include "SuperLeaf.h"
using namespace Utils;
using namespace Entities;

std::string ContentId::NONE = "None";

const std::unordered_map<std::string, ContentFactory::CreateItemMethod> ContentFactory::createMethodByContentId =
{
	{"Coin", &ContentFactory::CreateCoin},
	{"Mushroom", &ContentFactory::CreateMushroom},
	{"SuperLeaf",&ContentFactory::CreateSuperLeaf}
};

ContentFactory::ContentFactory(LPMario player)
	: player(player)
{
}

LPEntity ContentFactory::Create(const std::string& itemName, const Vector2<float>& position)
{
	//TODO: remove this line of code when everything is implemented
	if (!Contains(itemName, createMethodByContentId))
		return CreateCoin(position);

	return  (this->*createMethodByContentId.at(itemName))(position);
}

LPEntity ContentFactory::CreateCoin(const Utils::Vector2<float>& position)
{
	return new CoinUp(position);
}

LPEntity ContentFactory::CreateMushroom(const Utils::Vector2<float>& position)
{
	return new Mushroom(position);
}

LPEntity ContentFactory::CreateSuperLeaf(const Utils::Vector2<float>& position)
{
	if (player->GetPowerLevel() == PlayerPowerLevel::SMALL)
		return new Mushroom(position);
	else
		return new SuperLeaf(position);

}
