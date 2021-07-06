#pragma once
#include "Entity.h"
#include "Utils.h"

namespace Entities {
	class Wing : public Entity
	{
	public:
		enum class Direction {
			LEFT, RIGHT
		};

		Wing(LPEntity whoToFollow);
		Wing(LPEntity whoToFollow, Direction dir, const Utils::Vector2<float>& offset);
		void Update(float delta) override;
		void AutoFlap();
		void SetDirection(Direction dir);
		void SetOffset(const Utils::Vector2<float>& offset);
		void SetFlapSpeed(float speed);
		void FlapUp();
		void FlapDown();
		void Destroy();

	private:
		LPEntity target;
		Utils::Vector2<float> offset;
		std::string dirCode;
		float animSpeed;
	};
}

