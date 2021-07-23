#pragma once
#include "Entity.h"
#include "Direction.h"

namespace Entities {
	class PlayerTransitionPipe : public Entity
	{
	public:
		PlayerTransitionPipe(VDirection direction);
		void OnReady() override;
		void Update(float delta) override;

	private:
		LPEntity targetPlayer;
		VDirection dir;
		float endY;
		float velY;
		float time;

		static const float SPEED;
		static const float WAIT_DURATION;
	};
}

