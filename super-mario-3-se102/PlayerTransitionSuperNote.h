#pragma once
#include "Entity.h"
#include "Direction.h"

namespace Entities {
	class PlayerTransitionSuperNote : public Entity
	{
	public:
		PlayerTransitionSuperNote(LPEntity player);
		void OnReady() override;

	private:
		static const float SPEED;
	};
}

