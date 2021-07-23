#pragma once
#include "Entity.h"
#include "Direction.h"

namespace Entities {
	class PlayerTransitionSuperNote : public Entity
	{
	public:
		PlayerTransitionSuperNote();
		void OnReady() override;

	private:
		static const float SPEED;
	};
}

