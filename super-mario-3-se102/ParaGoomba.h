#pragma once
#include "Entity.h"
#include "Utils.h"
#include "CollisionEngine.h"
#include "EntityState.h"
#include "Wing.h"
#include "Direction.h"
#include "Color.h"

namespace Entities {
	class ParaGoomba : public Entity
	{
	public:
		~ParaGoomba();
		ParaGoomba(const std::string& colorType, const Utils::Vector2<float>& position);
		void Update(float delta) override;
		void Render() override;
		void KnockOver(HDirection direction);

	private:
		virtual void OnReady() override;
		void OnCollision(CollisionData data);

		void MoveAround(float delta);
		void PrepareToJump(float delta);
		void Jump(float delta);
		void Fall(float delta);
		void StompedOn(float delta);

		bool onGround;
		int jumpCount;
		std::string colorCode;
		std::string colorType;
		float time;
		Entities::Wing wingLeft;
		Entities::Wing wingRight;
		EventHandler<CollisionData> onCollisionHandler;
		EntityState<ParaGoomba> state;
		static const float PREPARE_JUMP_SPEED;
		static const float JUMP_SPEED;
		static const float TIME_TILL_PREPARE;
		static const int NUM_OF_PREPARE_JUMPS;
		static const float JUMP_FLAP_ANIM_SPEED;
		static const float FALL_SPEED;
	};
}