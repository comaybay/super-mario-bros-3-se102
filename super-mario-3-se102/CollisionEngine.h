#pragma once
#include "Utils.h"
#include "Entity.h"
#include "Event.h"
#include <unordered_set>

struct CollisionData {
	LPEntity who = nullptr;
	Utils::Vector2<int> edge = { 0, 0 };
	float value = 1.0f;
	float delta = 0;

	CollisionData();
	CollisionData(LPEntity who, Utils::Vector2<float> edge, float value, float delta);
};

/// <summary>
/// Data needed to calculate collision of entities in a scene
/// </summary>
struct CollisionEngineData {
	std::unordered_map<LPEntity, LPEvent<CollisionData>> collisionEventByLPEntity;
	std::unordered_map<LPEntity, EntityGroups> targetGroupsByMovableLPEntity;
	std::unordered_map<LPEntity, EntityGroups> targetGroupsByNonMovingLPEntity;
};

class CollisionEngine
{
private:
	struct CBox;

public:
	template<class ENTITY, class ...ARGS>
	static void Subscribe(ENTITY* handlerThis, void(ENTITY::* handler)(ARGS...), EntityGroups targetEntityGroups);

	template<class ENTITY, class ...ARGS>
	static void Unsubscribe(ENTITY* handlerThis, void(ENTITY::* handler)(ARGS...));

	static void Update(float delta);
	static bool Detect(LPEntity e1, LPEntity e2, float delta, CollisionData& dataForE1, CollisionData& dataForE2);
	static float DetectCollisionValue(LPEntity e1, LPEntity e2, float delta);

	/// <summary>
	/// Add new CollisionEngineData for a scene. Used internally by SceneLoader
	/// </summary>
	static void _AddCED(LPScene scene);

	/// <summary>
	/// Remove CollisionEngineData of a scene. Used internally by SceneLoader
	/// </summary>
	static void _RemoveCED(LPScene scene);

	/// <summary>
	/// Set Active CollisionEngineData. Used internally by Game
	/// </summary>
	static void _SetActiveCED(LPScene scene);

	/// <summary>
	/// <para> Remove entities inside of unsubscribeWaitList from collision event. Used internally by Game </para>
	/// <para> This method must be called after Scene Update method, before CollisionEngine Update method and before 
	/// scene switching in order to prevent deleted entities or entities that are in previously active CED inside of unsubscribeWaitList</para>
	/// </summary>
	static void _HandleUnsubscribeWaitList();

private:
	static void DetectAndNotify(LPEntity entity, const EntityGroups& targetGroups, float delta);
	static Event<CollisionData>& GetCollisionEventOf(LPEntity entity);
	static CollisionData SweptAABB(const CBox& mBox, const CBox& sBox);
	static bool AABBCheck(const CBox& box1, const CBox& box2);
	static CBox GetSweptBroadphaseBox(const CBox& box);
	static void OnEntityUnsubscribe(LPEntity entity);
	static std::unordered_map<LPScene, CollisionEngineData> CEDByLPScene;
	static CollisionEngineData* activeCED;
	static std::unordered_set<LPEntity> unsubscribeWaitList;
	static std::unordered_set<std::string> hasPreviouslyNotified;

	struct CBox {
		Utils::Vector2<float> position;
		Utils::Dimension<float> dimension;
		Utils::Vector2<float> velocity;

		CBox() {};
		CBox(Utils::Vector2<float> position, Utils::Dimension<float> dimension, Utils::Vector2<float> velocity)
			: position(position), dimension(dimension), velocity(velocity)
		{};
	};
};

template<class ENTITY, class ...ARGS>
static void CollisionEngine::Subscribe(ENTITY* handlerThis, void(ENTITY::* handler)(ARGS...), EntityGroups targetEntityGroups) {
	//entites that have group in one of collisionTargetGroups will be check for collision with entity
	if (handlerThis->GetGridType() == GridType::MOVABLE_ENTITIES || handlerThis->GetGridType() == GridType::NONE)
		CEDByLPScene[handlerThis->GetParentScene()].targetGroupsByMovableLPEntity[handlerThis] = targetEntityGroups;
	else
		CEDByLPScene[handlerThis->GetParentScene()].targetGroupsByNonMovingLPEntity[handlerThis] = targetEntityGroups;

	GetCollisionEventOf(handlerThis).Subscribe(handlerThis, handler);
	handlerThis->GetDestroyEvent().Subscribe(&OnEntityUnsubscribe);
}

template<class ENTITY, class ...ARGS>
static void CollisionEngine::Unsubscribe(ENTITY* handlerThis, void(ENTITY::* handler)(ARGS...)) {
	GetCollisionEventOf(handlerThis).Unsubscribe(handlerThis, handler);
	handlerThis->GetDestroyEvent().Unsubscribe(&OnEntityUnsubscribe);

	unsubscribeWaitList.insert(handlerThis);
}

