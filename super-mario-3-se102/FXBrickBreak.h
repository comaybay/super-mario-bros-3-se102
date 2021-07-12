#pragma once
#include "Utils.h"
#include "Entity.h"
#include <array>

namespace Entities {
	class FXBrickBreak : public Entity
	{
	public:
		class BrickBreakParticle : public Entity {
		public:
			BrickBreakParticle(const Utils::Vector2<float>& position, const Utils::Vector2<float>& velocity);
			void Update(float delta) override;
		};

	public:
		FXBrickBreak(const Utils::Vector2<float>& position);
		~FXBrickBreak();
		void Update(float delta) override;
		void Render() override;

	private:
		//for some reason, when allocate BrickBreakParticle on the stack, it immediately called it's
		//destructor before destroyEvent even get assigned, causing an error.
		std::array<LPEntity, 4> particles;
		float time;
		static const float UP_SPEED;
		static const float X_SPEED;
		static const float MARGIN;
		static const float EXIST_DURATION;
	};
}


