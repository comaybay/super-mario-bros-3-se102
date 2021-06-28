#include "WMNode.h"
#include "Game.h"
using namespace Entities;
using namespace Utils;

const float WMNode::TRANSFER_SPEED = 32;

WMNode::WMNode()
	: Entity::Entity({ -1, -1 }, "Nodes", GridType::STATIC_ENTITIES),
	isActiveNode(false),
	state(EntityState<WMNode>(this, &WMNode::Inactive))
{
}

void WMNode::_Init(const Utils::Vector2<float>& position, const std::string& scenePath,
	LPWMNode topNode, LPWMNode leftNode, LPWMNode bottomNode, LPWMNode rightNode)

{
	this->position = position;
	this->scenePath = scenePath;
	this->topNode = topNode;
	this->leftNode = leftNode;
	this->bottomNode = bottomNode;
	this->rightNode = rightNode;
}


void WMNode::Update(float delta)
{
	Entity::Update(delta);
	state.Handle(delta);
}

void WMNode::Activate(LPEntity wmPlayer, const Utils::Vector2<float> offset)
{
	isActiveNode = true;
	this->wmPlayer = wmPlayer;
	playerOffset = offset;
}

void WMNode::Inactive(float delta)
{
}

void WMNode::Active(float delta)
{
	if (!isActiveNode)
		return;

	if (topNode && Game::IsKeyPressed(DIK_UP)) {
		state.SetHandler(&WMNode::TransferAnimTop);
	}
	else if (leftNode && Game::IsKeyPressed(DIK_LEFT)) {
		state.SetHandler(&WMNode::TransferAnimLeft);
	}
	else if (bottomNode && Game::IsKeyPressed(DIK_DOWN)) {
		state.SetHandler(&WMNode::TransferAnimDown);
	}
	else if (rightNode && Game::IsKeyPressed(DIK_RIGHT)) {
		state.SetHandler(&WMNode::TransferAnimRight);
	}
}

void WMNode::TransferAnimTop(float delta)
{
	Vector2<float> pos = wmPlayer->GetPosition();
	float destY = topNode->GetPosition().y + playerOffset.y;
	pos.y -= TRANSFER_SPEED;

	if (pos.y < destY) {
		pos.y = destY;
		Transfer(topNode);
	}

	wmPlayer->SetPosition(pos);
}

void WMNode::TransferAnimLeft(float delta)
{
	Vector2<float> pos = wmPlayer->GetPosition();
	float destX = leftNode->GetPosition().x + playerOffset.x;
	pos.x -= TRANSFER_SPEED;

	if (pos.x < destX) {
		pos.x = destX;
		Transfer(leftNode);
	}

	wmPlayer->SetPosition(pos);
}


void WMNode::TransferAnimDown(float delta)
{
	Vector2<float> pos = wmPlayer->GetPosition();
	float destY = bottomNode->GetPosition().y + playerOffset.y;
	pos.y += TRANSFER_SPEED;

	if (pos.y > destY) {
		pos.y = destY;
		Transfer(bottomNode);
	}

	wmPlayer->SetPosition(pos);
}


void WMNode::TransferAnimRight(float delta)
{
	Vector2<float> pos = wmPlayer->GetPosition();
	float destX = rightNode->GetPosition().x + playerOffset.x;
	pos.y -= TRANSFER_SPEED;

	if (pos.x > destX) {
		pos.x = destX;
		Transfer(rightNode);
	}

	wmPlayer->SetPosition(pos);
}

void WMNode::Transfer(LPWMNode targetNode)
{
	targetNode->Activate(wmPlayer, playerOffset);
	isActiveNode = false;
	wmPlayer = nullptr;
	playerOffset = { 0, 0 };
}

