#include "WMNode.h"
#include "Game.h"
using namespace Entities;
using namespace Utils;

const float WMNode::TRANSFER_SPEED = 128;

WMNode::WMNode()
	: Entity::Entity({ -1, -1 }, "Nodes", GridType::STATIC_ENTITIES),
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

	if (wmPlayer != nullptr)
		wmPlayer->Update(delta);
}

void WMNode::Render()
{
	if (wmPlayer != nullptr)
		wmPlayer->Render();
}

void WMNode::Activate(LPEntity wmPlayer)
{
	this->wmPlayer = wmPlayer;
	wmPlayer->SetPosition(position);
	state.SetHandler(&WMNode::Active);
}

void WMNode::Inactive(float delta)
{
}

void WMNode::Active(float delta)
{
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
	float destY = topNode->GetPosition().y;
	pos.y -= TRANSFER_SPEED * delta;
	wmPlayer->SetPosition(pos);

	if (pos.y > destY)
		return;

	pos.y = destY;
	wmPlayer->SetPosition(pos);
	Transfer(topNode);
}

void WMNode::TransferAnimLeft(float delta)
{
	Vector2<float> pos = wmPlayer->GetPosition();
	float destX = leftNode->GetPosition().x;
	pos.x -= TRANSFER_SPEED * delta;
	wmPlayer->SetPosition(pos);

	if (pos.x > destX)
		return;

	pos.x = destX;
	wmPlayer->SetPosition(pos);
	Transfer(leftNode);
}


void WMNode::TransferAnimDown(float delta)
{
	Vector2<float> pos = wmPlayer->GetPosition();
	float destY = bottomNode->GetPosition().y;
	pos.y += TRANSFER_SPEED * delta;
	wmPlayer->SetPosition(pos);

	if (pos.y < destY)
		return;

	pos.y = destY;
	wmPlayer->SetPosition(pos);
	Transfer(bottomNode);
}


void WMNode::TransferAnimRight(float delta)
{
	Vector2<float> pos = wmPlayer->GetPosition();
	float destX = rightNode->GetPosition().x;
	pos.x += TRANSFER_SPEED * delta;
	wmPlayer->SetPosition(pos);

	if (pos.x < destX)
		return;

	pos.x = destX;
	wmPlayer->SetPosition(pos);
	Transfer(rightNode);
}

void WMNode::Transfer(LPWMNode targetNode)
{
	targetNode->Activate(wmPlayer);
	state.SetHandler(&WMNode::Inactive);
	wmPlayer = nullptr;
}

