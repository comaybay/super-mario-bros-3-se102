#include "Entity.h"
#include "EntityState.h"
#include "Utils.h"

namespace Entities {
	class EmptyBlock : public Entity {
	public:
		EmptyBlock(const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		void Idle(float _);
		void HitAnimation(float delta);

		EntityState<EmptyBlock> state;
		Utils::Vector2<float> initialPos;
		static const float UP_SPEED;
		static const float FALL_SPEED;
	};
}