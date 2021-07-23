#pragma once
#include "Entity.h"
#include "Event.h"
#include "Direction.h"
#include "Mario.h"

namespace Entities {
	class PlayerTransitionPipe : public Entity
	{
	public:
		PlayerTransitionPipe(LPMario player, VDirection direction, const Utils::Vector2<float>& offset = { 0, 0 });
		void OnReady() override;
		void Update(float delta) override;
		Event<>& GetTransitionCompleteEvent();

	private:
		Event<> transitionCompleteEvent;
		Utils::Vector2<float> offset;
		LPMario targetPlayer;
		VDirection dir;
		float endY;
		float velY;
		float time;

		static const float SPEED;
		static const float WAIT_DURATION;
	};
	typedef PlayerTransitionPipe* LPPlayerTransitionPipe;
}

