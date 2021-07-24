#include "MarioTransition.h"
#include "MarioSmall.h"
#include "MarioSuper.h"
#include "MarioRaccoon.h"
#include "Game.h"
#include "Constants.h"
#include "Group.h"
using namespace Entities;
using namespace Utils;

const float MarioTransition::STB_PART1_DURATION = 0.5f;
const float MarioTransition::STB_PART2_DURATION = 0.3f;
const float MarioTransition::SMOKE_DURATION = 0.48f;

MarioTransition::MarioTransition(LPMario player, PlayerPowerLevel to)
	: Entity(player->GetPosition(), { "PlayerTransitions", Group::NOT_AFFECTED_BY_TRANSITION_PAUSE }, GridType::NONE),
	player(player),
	state(EntityState<MarioTransition>(this)),
	time(0)
{

	player->Activate(false);

	animDirCode = player->GetFacingDirection() == HDirection::LEFT ? "Left" : "Right";
	PlayerPowerLevel from = player->GetPowerLevel();

	using PPL = ::PlayerPowerLevel;
	//Small to Big
	if (from == PPL::SMALL) {
		position.y -= Constants::TILE_SIZE;
		isDowngradeTransiton = false;
		resultPowerLevel = PlayerPowerLevel::BIG;
		SetAnimation("MarioTransition1" + animDirCode);
		state.SetState(&MarioTransition::SmallToBigPart1);
		return;
	}

	//Big to Small
	if (from == PPL::BIG && to == PPL::SMALL) {
		isDowngradeTransiton = true;
		resultPowerLevel = PlayerPowerLevel::SMALL;
		SetAnimation("MarioTransition2" + animDirCode);
		state.SetState(&MarioTransition::BigToSmallPart1);
		return;
	}

	//downgrade to Big or upgrade from Big
	isDowngradeTransiton = (to == PPL::BIG) ? true : false;
	resultPowerLevel = to;
	state.SetState(&MarioTransition::Smoke);
	SetAnimation("FXSmoke");

}

void MarioTransition::OnReady()
{
	Entity::OnReady();
	parentScene->UnsubscribeToOutOfWorldEvent(this);
	parentScene->GetCamera().StopFollowingEntity();
	parentScene->TransitionPause(true);
}

void MarioTransition::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);

	if (isDowngradeTransiton)
		invincibleTime += delta;
}

void MarioTransition::Render()
{
	if (!isDowngradeTransiton) {
		Entity::Render();
		return;
	}

	static const int maxFPS = Game::GetGameSettings().maxFPS;

	if (int(invincibleTime * maxFPS / Mario::FLASH_DURATION) % 2)
		Entity::Render();
}

void MarioTransition::SmallToBigPart1(float delta)
{
	time += delta;

	if (time >= STB_PART1_DURATION) {
		time = 0;
		SetAnimation("MarioTransition2" + animDirCode);
		state.SetState(&MarioTransition::SmallToBigPart2);
	};
}

void MarioTransition::SmallToBigPart2(float delta)
{
	time += delta;

	if (time >= STB_PART2_DURATION) {
		time = 0;
		CreateMario();
		parentScene->QueueFree(this);
	}
}

void MarioTransition::BigToSmallPart1(float delta)
{
	time += delta;

	if (time >= STB_PART1_DURATION) {
		time = 0;
		SetAnimation("MarioTransition1" + animDirCode);
		state.SetState(&MarioTransition::SmallToBigPart2);
	};
}

void MarioTransition::BigToSmallPart2(float delta)
{
	time += delta;

	if (time >= STB_PART2_DURATION) {
		time = 0;
		CreateMario();
		parentScene->QueueFree(this);
	};
}

void MarioTransition::Smoke(float delta)
{
	time += delta;

	if (time >= SMOKE_DURATION) {
		CreateMario();
		parentScene->QueueFree(this);
	}
}

void MarioTransition::CreateMario()
{
	parentScene->QueueFree(player);

	using PPL = ::PlayerPowerLevel;
	LPMario newPlayer = nullptr;

	//TODO: complete the implementation
	switch (resultPowerLevel) {
	case PPL::SMALL:
		newPlayer = new MarioSmall({ position.x, position.y + Constants::TILE_SIZE }, player->GetFacingDirection());
		break;

	case PPL::BIG:
		newPlayer = new MarioSuper(position, player->GetFacingDirection());
		break;

	case PPL::RACCOON:
		newPlayer = new MarioRaccoon(position, player->GetFacingDirection());
	}

	if (isDowngradeTransiton) {
		newPlayer->TurnInvinsible();
	}

	const Vector2<float>& newPlayerVel = player->GetVelocity();
	parentScene->AddEntity(newPlayer,
		[newPlayerVel](LPEntity player) {
			player->SetVelocity(newPlayerVel);
		}
	);

	if (resultPowerLevel == PPL::SMALL)
		parentScene->GetCamera().FollowEntity(newPlayer);
	else
		parentScene->GetCamera().FollowEntity(newPlayer, { 0, (float)Constants::TILE_SIZE });

	parentScene->TransitionPause(false);
}

