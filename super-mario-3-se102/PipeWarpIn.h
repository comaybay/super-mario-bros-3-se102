#pragma once
#include "CollisionEngine.h"
#include "Direction.h"

namespace Entities {
	class PipeWarpIn : public Entity
	{
	public:
		PipeWarpIn(const std::string& scenePath, VDirection inDirection, const Utils::Vector2<float>& position);
		void OnReady() override;

	private:
		std::string scenePath;
		VDirection dir;
		void OnCollision(CollisionData data);
		void OnTransitionComplete();
	};
}

