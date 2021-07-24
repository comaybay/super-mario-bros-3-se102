#pragma once
#include "CollisionEngine.h"
#include "Direction.h"

namespace Entities {
	class PipeWarpOut : public Entity
	{
	public:
		PipeWarpOut(const std::string& scenePath, VDirection outDirection, const Utils::Vector2<float>& position);
		void OnReady() override;

	private:
		void OnTransitionComplete();

		std::string scenePath;
		VDirection dir;
	};
}

