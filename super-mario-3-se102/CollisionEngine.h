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

	class OnEntityUnsubscribeHandler {
	public:
		void Handle(LPEntity entity);
	};

public:
	template<class TEntity, class ...Args>
	static void Subscribe(TEntity* handlerThis, void(TEntity::* handler)(Args...), std::vector<std::string> collisionTargetGroups);

	template<class TEntity, class ...Args>
	static void Unsubscribe(TEntity* handlerThis, void(TEntity::* handler)(Args...));

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
	static OnEntityUnsubscribeHandler onEntityUnsubscribe;
	static std::list<LPEntity> unsubscribeWaitList;

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
	//entites in these groups will be check for collision with entity
	targetGroupsByLPEntity[handlerThis] = collisionTargetGroups;
	GetCollisionEventOf(handlerThis)->Subscribe(handlerThis, handler);
	handlerThis->GetDestroyEvent()->Subscribe(&onEntityUnsubscribe, &OnEntityUnsubscribeHandler::Handle);
}

template<class TEntity, class ...Args>
static void CollisionEngine::Unsubscribe(TEntity* handlerThis, void(TEntity::* handler)(Args...)) {
	GetCollisionEventOf(handlerThis)->Unsubscribe(handlerThis, handler);
	handlerThis->GetDestroyEvent()->Unsubscribe(&onEntityUnsubscribe, &OnEntityUnsubscribeHandler::Handle);

	unsubscribeWaitList.push_back(handlerThis);
}

