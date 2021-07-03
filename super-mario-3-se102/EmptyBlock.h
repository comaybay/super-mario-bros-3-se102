#include "Entity.h"
#include "BlockHitMovement.h"
#include "EntityState.h"

namespace Entities {
	class EmptyBlock : public Entity {
	public:
		EmptyBlock(const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		void Idle(float _);
		void HitAnimation(float delta);

		EntityState<EmptyBlock> state;
		LPBlockHitMovement blockHitMovement;
	};
}