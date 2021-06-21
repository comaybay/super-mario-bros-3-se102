#pragma once
#include "Utils.h"
#include "Entity.h"
#include "Event.h"

struct CollisionData {
	LPEntity who = nullptr;
	Utils::Vector2<float> edge = Utils::Vector2<float>(0, 0);
	float value = 1.0f;
	float delta = 0;

	CollisionData();
	CollisionData(LPEntity who, Utils::Vector2<float> edge, float value, float delta);
};

class CollisionEngine
{
private:
	struct CBox;

public:
	template<class TEntity, class ...Args>
	static void Subscribe(TEntity* handlerThis, void(TEntity::* handler)(Args...), std::vector<std::string> collisionTargetGroups);

	template<class TEntity, class ...Args>
	static void Unsubscribe(TEntity* handlerThis, void(TEntity::* handler)(Args...));

	static void Update(float delta);
	static void DetectAndNotify(LPEntity entity, const std::vector<std::string>& targetGroups, float delta);
	static bool Detect(LPEntity e1, LPEntity e2, float delta, CollisionData& dataForE1, CollisionData& dataForE2);
	static float DetectCollisionValue(LPEntity e1, LPEntity e2, float delta);
private:
	static Event<CollisionData>& GetCollisionEventOf(LPEntity entity);
	static CollisionData SweptAABB(const CBox& mBox, const CBox& sBox);
	static bool AABBCheck(const CBox& box1, const CBox& box2);
	static CBox GetSweptBroadphaseBox(const CBox& box);
	static void OnEntityDestroy(LPEntity entity);
	static void OnEntityUnsubscribe(LPEntity entity);
	static std::unordered_map<LPEntity, LPEvent<CollisionData>> collisionEventByLPEntity;
	static std::unordered_map<LPEntity, std::vector<std::string>> targetGroupsByMovableLPEntity;
	static std::unordered_map<LPEntity, std::vector<std::string>> targetGroupsByNonMovingLPEntity;
	static std::list<LPEntity> unsubscribeWaitList;
	static std::unordered_set<std::string> hasPreviouslyNotified;

	struct CBox {
		Utils::Vector2<float> position;
		Utils::Dimension dimension;
		Utils::Vector2<float> velocity;

		CBox() {};
		CBox(Utils::Vector2<float> position, Utils::Dimension dimension, Utils::Vector2<float> velocity)
			: position(position), dimension(dimension), velocity(velocity)
		{};
	};
};

template<class TEntity, class ...Args>
static void CollisionEngine::Subscribe(TEntity* handlerThis, void(TEntity::* handler)(Args...), std::vector<std::string> collisionTargetGroups) {
	//entites that have group in one of collisionTargetGroups will be check for collision with entity
	if (handlerThis->GetGridType() == GridType::MOVABLE_ENTITIES || handlerThis->GetGridType() == GridType::NONE)
		targetGroupsByMovableLPEntity[handlerThis] = collisionTargetGroups;
	else
		targetGroupsByNonMovingLPEntity[handlerThis] = collisionTargetGroups;

	GetCollisionEventOf(handlerThis).Subscribe(handlerThis, handler);
	handlerThis->GetDestroyEvent().Subscribe(&OnEntityUnsubscribe);
}

template<class TEntity, class ...Args>
static void CollisionEngine::Unsubscribe(TEntity* handlerThis, void(TEntity::* handler)(Args...)) {
	GetCollisionEventOf(handlerThis).Unsubscribe(handlerThis, handler);
	handlerThis->GetDestroyEvent().Unsubscribe(&OnEntityUnsubscribe);

	unsubscribeWaitList.push_back(handlerThis);
}

