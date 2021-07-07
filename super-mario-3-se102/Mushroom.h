#pragma once
#include "EntityState.h"
#include "Entity.h"

namespace Entities {
	class Mushroom : public Entity
	{
	public:
		Mushroom(const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		void RiseUp(float delta);
		void Move(float delta);

		EntityState<Mushroom> state;
		float riseDestY;
		static const float MOVE_SPEED;
		static const float RISE_SPEED;
	};
}