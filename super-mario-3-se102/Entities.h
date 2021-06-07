#pragma once
#include "Entity.h"
#include "CollisionDetection.h"

namespace Entities {
	using namespace Utils;

	class CollisionWallType1 : public Entity {
	public:
		CollisionWallType1(Vector2 position, Dimension dimension);
		Dimension GetDimension() override;

	private:
		Dimension dim;
	};

	class CollisionWallType2 : public Entity {
	public:
		CollisionWallType2(Vector2 position, Dimension dimension);
		Dimension GetDimension() override;

	private:
		Dimension dim;
	};

	class Mario : public Entity {
	public:
		Mario(Vector2 position);
		void Update(float delta) override;
	private:
		void OnCollision(CollisionData data);
		void WallSide(CollisionData& data);

		void SwitchState(void (Mario::* state)(float delta));
		void IdleUpdate(float delta);
		void WalkUpdate(float delta);
		void RunUpdate(float delta);
		void JumpUpdate(float delta);
		void FallUpdate(float delta);

		EventHandler<Mario, CollisionData> onCollision;
		void (Mario::* pStateUpdate)(float delta) = NULL;
		Utils::Vector2 dir;
		float jumpDuration;
		static const Utils::Vector2 acceleration;
		static const Utils::Vector2 maxSpeed;
		bool onGround;
		int prevPressedKeyHorizontal;
		static const float jumpSpeed;
		static const float jumpSpeedAfterMaxWalkSpeed;
		static const float jumpSpeedReleaseEarly;
	};

	class Goomba : public Entity
	{
	public:
		Goomba(std::string color, Vector2 position);
		void Update(float delta) override;
	private:
		std::string colorCode;
		Vector2 speed;
		static float maxFallSpeed;
		EventHandler<CollisionData> onCollisionHandler;
		void OnCollision(CollisionData data);
	};

	class QuestionBlock : public Entity {
	public:
		QuestionBlock(LPEntity content, const Vector2& position);
		void Update(float delta) override;
	private:
		void OnCollision(CollisionData data);
		LPEntity content;
	};

	class Coin : public Entity {
	public:
		Coin(const Vector2& position);
		void OnCollision(CollisionData data);
	};

}