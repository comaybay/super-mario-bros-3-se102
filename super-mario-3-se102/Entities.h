#pragma once
#include "Entity.h"
#include "CollisionEngine.h"

namespace Entities {
	using namespace Utils;

	class CollisionWallType1 : public Entity {
	public:
		CollisionWallType1(Vector2<float> position, Dimension dimension);
		Dimension GetDimension() override;

	private:
		Dimension dim;
	};

	class CollisionWallType2 : public Entity {
	public:
		CollisionWallType2(Vector2<float> position, Dimension dimension);
		Dimension GetDimension() override;

	private:
		Dimension dim;
	};

	class Mario : public Entity {
	public:
		Mario(Vector2<float> position);
		void Update(float delta) override;
	private:
		void OnCollision(CollisionData data);
		void WallSlide(CollisionData& data);
		void UpdateHorizontalDirection(float delta);

		void SwitchState(void (Mario::* state)(float delta));
		void Idle(float delta);
		void Walk(float delta);
		void Jump(float delta);
		void Fall(float delta);
		void DieWait(float delta);
		void DieFall(float delta);

		EventHandler<Mario, CollisionData> onCollision;
		void (Mario::* state)(float delta);
		Utils::Vector2<float> dir;
		float time;
		bool onGround;
		int prevPressedKeyHorizontal;
		static const float MAX_FALL_SPEED;
		static const float MAX_WALK_SPEED;
		static const float MAX_RUN_SPEED;
		static const Utils::Vector2<float> ACCELERATION;
		static const float JUMP_SPEED;
		static const float JUMP_SPEED_AFTER_MAX_WALK_SPEED;
		static const float JUMP_SPEED_RELASE_EARLY;
		static const float DEATH_JUMP_SPEED;
	};

	class Goomba : public Entity
	{
	public:
		Goomba(std::string color, Vector2<float> position);
		void Update(float delta) override;
	private:
		std::string colorCode;
		Vector2<float> speed;
		static float maxFallSpeed;
		EventHandler<CollisionData> onCollisionHandler;
		virtual void OnReady() override;
		void OnCollision(CollisionData data);
	};

	class QuestionBlock : public Entity {
	public:
		QuestionBlock(LPEntity content, const Vector2<float>& position);
		void Update(float delta) override;
	private:
		void OnCollision(CollisionData data);
		LPEntity content;
	};

	class Coin : public Entity {
	public:
		Coin(const Vector2<float>& position);
		void OnCollision(CollisionData data);
	};

}