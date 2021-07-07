#pragma once
#include "Entity.h"
#include "Utils.h"

namespace Entities {
	enum class WingDirection {
		LEFT, RIGHT
	};

	class Wing : public Entity
	{
	public:
		Wing(LPEntity whoToFollow);
		Wing(LPEntity whoToFollow, WingDirection dir, const Utils::Vector2<int>& offset);
		void Update(float delta) override;
		void AutoFlap();
		void SetDirection(WingDirection dir);
		void SetOffset(const Utils::Vector2<float>& offset);
		void SetFlapSpeed(float speed);
		void FlapUp();
		void FlapDown();
		void Destroy();

	private:
		LPEntity target;
		Utils::Vector2<int> offset;
		std::string dirCode;
		float animSpeed;
	};
}

