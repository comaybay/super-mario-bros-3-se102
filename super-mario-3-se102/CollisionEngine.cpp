#include "CollisionEngine.h"
#include "EntityManager.h"
#include "Game.h"
#include "Groups.h"
#include <algorithm>
#include <unordered_set>
using namespace Utils;

CollisionData::CollisionData() {};

CollisionData::CollisionData(LPEntity who, Vector2<float> edge, float value, float delta)
	: who(who), edge(edge), value(value), delta(delta) {};

std::unordered_map<LPEntity, LPEvent<CollisionData>> CollisionEngine::collisionEventByLPEntity;
std::unordered_map<LPEntity, std::vector<std::string>> CollisionEngine::targetGroupsByLPEntity;
CollisionEngine::OnEntityUnsubscribeHandler CollisionEngine::onEntityUnsubscribe;
std::list<LPEntity> CollisionEngine::unsubscribeWaitList;


/// <summary>
/// <para>This algorithm do collision detection on entities that subsribe to the CollisionEngine, and only do detection between entity and it's given target groups.</para>
/// <para>The downside of this algorithm is that it do not take into account the sptial partition grid.</para>
/// <para>But because of that, you do not need to take into account collision between entities outside of active cells in spatial partition grid.</para>
/// <para>By using entity group partition, this algorithm should be fast enough for game with minimal ammount of objects 
/// and do not simmulate complex particle interactions, like SMB3.</para>
/// </summary>
void CollisionEngine::Update(float delta) {
	//keep events and targetGroups up to date before doing collision detection
	for (LPEntity entity : unsubscribeWaitList)
		onEntityUnsubscribe.Handle(entity);

	unsubscribeWaitList.clear();

	for (auto& pair : targetGroupsByLPEntity) {
		LPEntity entity = pair.first;
		//get target entities (use set data structure to avoid duplications)
		std::unordered_set<LPEntity> targetSet;
		for (std::string& groupName : pair.second)
			for (const LPEntity& target : Game::GetActiveScene()->GetEntitiesByGroup(groupName))
				if (target != entity)
					targetSet.insert(target);

		std::vector<LPEntity> targetEntities(targetSet.begin(), targetSet.end());

		//sort target entities by time of collision/closeness
		auto ascending = [entity, delta](const LPEntity& a, const LPEntity& b) -> bool {
			float aVal = CollisionEngine::DetectCollisionValue(entity, a, delta);
			float bVal = CollisionEngine::DetectCollisionValue(entity, b, delta);

			//if equal and time = 0, sort by distance to entity
			if (aVal == bVal && aVal != 1.0f) {
				Vector2<float> ePos = entity->GetPosition();
				return a->GetPosition().DistanceTo(ePos) < b->GetPosition().DistanceTo(ePos);
			}
			//sort by time of collision
			else
				return aVal < bVal;
		};
		std::sort(targetEntities.begin(), targetEntities.end(), ascending);
		std::unordered_set<std::string> hasPreviouslyNotified;
		auto toKey = [](LPEntity a, LPEntity b) -> std::string {
			return reinterpret_cast<const char*>(a) + std::string(",") + reinterpret_cast<const char*>(b);
		};
		//notify both if collided, need to notify right away so the next detections can give accurate results
		for (const LPEntity& target : targetEntities) {
			if (SetHas(toKey(entity, target), hasPreviouslyNotified))
				continue;

			CollisionData dataForEntity;
			CollisionData dataForTarget;
			CollisionEngine::Detect(entity, target, delta, dataForEntity, dataForTarget);

			if (dataForEntity.value == 1.0f)
				continue;
			collisionEventByLPEntity[entity]->Notify(dataForEntity);
			//if target also subscribed and it's target groups include entity
			if (MapHas(target, collisionEventByLPEntity) && VectorHasAnyOf(entity->GetEntityGroups(), targetGroupsByLPEntity[target])) {
				collisionEventByLPEntity[target]->Notify(dataForTarget);
				hasPreviouslyNotified.insert(toKey(target, entity));
			}
		}
	}
}

LPEvent<CollisionData> CollisionEngine::GetCollisionEventOf(LPEntity entity)
{
	if (!Utils::MapHas(entity, collisionEventByLPEntity))
		collisionEventByLPEntity[entity] = new Event<CollisionData>();

	return collisionEventByLPEntity[entity];
}

void CollisionEngine::Detect(LPEntity e1, LPEntity e2, float delta, CollisionData& dataForE1, CollisionData& dataForE2)
{
	CBox mBox(
		e1->GetPosition() + e1->GetHitbox()->GetRelativePosition(),
		e1->GetHitbox()->GetDimension(),
		(e1->_GetRemainingVelocity() - e2->_GetRemainingVelocity()) * delta
	);

	CBox sBox(
		e2->GetPosition() + e2->GetHitbox()->GetRelativePosition(),
		e2->GetHitbox()->GetDimension(),
		Utils::Vector2<float>(0, 0)
	);

	//Broadphase check
	CBox mbb = GetSweptBroadphaseBox(mBox);
	if (!AABBCheck(mbb, sBox))
		return;

	dataForE1 = SweptAABB(mBox, sBox);
	dataForE1.who = e2;
	dataForE1.delta = delta;

	dataForE2 = CollisionData(
		e1, -dataForE1.edge, 0, delta
	);
}

float CollisionEngine::DetectCollisionValue(LPEntity e1, LPEntity e2, float delta)
{
	CBox mBox(
		e1->GetPosition() + e1->GetHitbox()->GetRelativePosition(),
		e1->GetHitbox()->GetDimension(),
		(e1->_GetRemainingVelocity() - e2->_GetRemainingVelocity()) * delta
	);

	CBox sBox(
		e2->GetPosition() + e2->GetHitbox()->GetRelativePosition(),
		e2->GetHitbox()->GetDimension(),
		Utils::Vector2<float>(0, 0)
	);

	//Broadphase check
	CBox mbb = GetSweptBroadphaseBox(mBox);
	if (!AABBCheck(mbb, sBox))
		return 1.0f;
	else
		return SweptAABB(mBox, sBox).value;
}

//taken from https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/ 
CollisionEngine::CBox CollisionEngine::GetSweptBroadphaseBox(const CBox& box) {
	CBox b;
	b.position = Vector2<float>(
		box.velocity.x > 0 ? box.position.x : box.position.x + box.velocity.x,
		box.velocity.y > 0 ? box.position.y : box.position.y + box.velocity.y
		);

	b.dimension = Dimension(
		box.dimension.width + abs(box.velocity.x),
		box.dimension.height + abs(box.velocity.y)
	);
	return b;
}

void CollisionEngine::OnEntityUnsubscribeHandler::Handle(LPEntity entity)
{
	auto itCE = collisionEventByLPEntity.find(entity);
	collisionEventByLPEntity.erase(itCE);
	auto itTG = targetGroupsByLPEntity.find(entity);
	targetGroupsByLPEntity.erase(itTG);
}

//taken from https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/ with modifications
bool CollisionEngine::AABBCheck(const CBox& box1, const CBox& box2)
{
	//MODIFICATION: >= and <= instead of > and <.
	//if box1 is outside of box2, return false else true
	return !(
		box1.position.x + box1.dimension.width <= box2.position.x ||
		box1.position.x >= box2.position.x + box2.dimension.width ||
		box1.position.y + box1.dimension.height <= box2.position.y ||
		box1.position.y >= box2.position.y + box2.dimension.height
		);
}

//taken from https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/
CollisionData CollisionEngine::SweptAABB(const CBox& mBox, const CBox& sBox) {
	CollisionData data;
	Vector2<float> invEntry;
	Vector2<float> invExit;

	// find the distance between the objects on the near and far sides for both x and y 
	invEntry.x = sBox.position.x - (mBox.position.x + mBox.dimension.width);
	invExit.x = (sBox.position.x + sBox.dimension.width) - mBox.position.x;
	if (mBox.velocity.x < 0.0f)
		std::swap(invEntry.x, invExit.x);

	invEntry.y = sBox.position.y - (mBox.position.y + mBox.dimension.height);
	invExit.y = (sBox.position.y + sBox.dimension.height) - mBox.position.y;
	if (mBox.velocity.y < 0.0f)
		std::swap(invEntry.y, invExit.y);

	// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero) 
	Vector2<float> entryTimeAxis;
	Vector2<float> exitTimeAxis;

	if (mBox.velocity.x == 0) {
		entryTimeAxis.x = -std::numeric_limits<float>::infinity();
		exitTimeAxis.x = std::numeric_limits<float>::infinity();
	}
	else {
		entryTimeAxis.x = invEntry.x / mBox.velocity.x;
		exitTimeAxis.x = invExit.x / mBox.velocity.x;
	}

	if (mBox.velocity.y == 0) {
		entryTimeAxis.y = -std::numeric_limits<float>::infinity();
		exitTimeAxis.y = std::numeric_limits<float>::infinity();
	}
	else {
		entryTimeAxis.y = invEntry.y / mBox.velocity.y;
		exitTimeAxis.y = invExit.y / mBox.velocity.y;
	}

	// find the earliest/latest times of collision
	float entryTime = max(entryTimeAxis.x, entryTimeAxis.y); //both axes must intersect
	float exitTime = min(exitTimeAxis.x, exitTimeAxis.y); //one axis out of intersection

	// if there was no collision
	if (entryTime > exitTime || (entryTimeAxis.x < 0.0f && entryTimeAxis.y < 0.0f) || entryTimeAxis.x > 1.0f || entryTimeAxis.y > 1.0f)
	{
		data.value = 1.0f;
		return data;
	}

	//MODIFICATION: using velocity instead to avoid invertEntry.x or y equals 0 (because of large framerates or very small velocity)
	if (entryTimeAxis.x > entryTimeAxis.y)
	{
		if (mBox.velocity.x < 0.0f)
			data.edge.x = 1.0f;
		else
			data.edge.x = -1.0f;
	}
	else
	{
		if (mBox.velocity.y < 0.0f)
			data.edge.y = 1.0f;
		else
			data.edge.y = -1.0f;
	}

	data.value = entryTime;
	return data;
}
