#pragma once
#include "Utils.h"
#include "Entity.h"
#include "Event.h"

struct CollisionData {
	LPEntity who = nullptr;
	Utils::Vector2 edge = Utils::Vector2(0, 0);
	float value = 1.0f;
	float delta;

	CollisionData();
	CollisionData(LPEntity who, Utils::Vector2 edge, float value, float delta);
};

class CollisionDetection
{
private:
	struct CBox;

	class OnEntityDestroyHandler {
	public:
		void Handle(LPEntity entity);
	};

public:
	template<class TEntity, class ...Args>
	static void Subscribe(TEntity* handlerThis, void(TEntity::* handler)(Args...), std::vector<std::string> collisionTargetGroups);
	static void Update(float delta);
	static void Detect(LPEntity e1, LPEntity e2, float delta, CollisionData& dataForE1, CollisionData& dataForE2);
	static float DetectCollisionValue(LPEntity e1, LPEntity e2, float delta);
private:
	static LPEvent<CollisionData> GetCollisionEventOf(LPEntity entity);
	static CollisionData SweptAABB(const CBox& mBox, const CBox& sBox);
	static bool AABBCheck(const CBox& box1, const CBox& box2);
	static CBox GetSweptBroadphaseBox(const CBox& box);
	static void OnEntityDestroy(LPEntity entity);
	static std::map<LPEntity, LPEvent<CollisionData>> collisionEventByLPEntity;
	static std::map<LPEntity, std::vector<std::string>> targetGroupsByLPEntity;
	static OnEntityDestroyHandler onEntityDestroy;

	struct CBox {
		Utils::Vector2 position;
		Utils::Dimension dimension;
		Utils::Vector2 velocity;

		CBox() {};
		CBox(Utils::Vector2 position, Utils::Dimension dimension, Utils::Vector2 velocity)
			: position(position), dimension(dimension), velocity(velocity)
		{};
	};
};

template<class TEntity, class ...Args>
static void CollisionDetection::Subscribe(TEntity* handlerThis, void(TEntity::* handler)(Args...), std::vector<std::string> collisionTargetGroups) {
	//entites in these groups will be check for collision with entity
	targetGroupsByLPEntity[handlerThis] = collisionTargetGroups;
	GetCollisionEventOf(handlerThis)->Subscribe(handlerThis, handler);
	handlerThis->GetDestroyEvent()->Subscribe(&onEntityDestroy, &OnEntityDestroyHandler::Handle);
}


