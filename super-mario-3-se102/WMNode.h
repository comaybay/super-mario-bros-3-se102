#pragma once
#include "Entity.h"
#include "EntityState.h"

namespace Entities {
	class WMNode;
	typedef WMNode* LPWMNode;

	class WMNode : public Entity
	{
	public:
		WMNode();

		/// <summary>
		/// initialize WMNode, this method exist because some operations require scene to exist before 
		/// it's constructor's arguments are provided, used internally by SceneManager.
		/// </summary>
		void _Init(const Utils::Vector2<float>& position, const std::string& scenePath,
			LPWMNode topNode, LPWMNode leftNode, LPWMNode bottomNode, LPWMNode rightNode);

		void Update(float delta) override;
		void SetScenePath(const std::string& scenePath);

		/// <summary>
		/// Set node to be the active node (where the player stand)
		/// </summary>
		void Activate(LPEntity wmPlayer, const Utils::Vector2<float> offset);

	private:
		void Inactive(float delta);
		void Active(float delta);
		void TransferAnimTop(float delta);
		void TransferAnimLeft(float delta);
		void TransferAnimDown(float delta);
		void TransferAnimRight(float delta);

		void Transfer(LPWMNode targetNode);

		EntityState<WMNode> state;
		std::string scenePath;
		bool isActiveNode;
		LPWMNode topNode;
		LPWMNode leftNode;
		LPWMNode bottomNode;
		LPWMNode rightNode;
		LPEntity wmPlayer;
		Utils::Vector2<float> playerOffset;
		static const float TRANSFER_SPEED;
	};
}
