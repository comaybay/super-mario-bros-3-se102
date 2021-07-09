#include "MarioTransition.h"
#include "MarioSmall.h"
#include "MarioBig.h"
#include "Game.h"
#include "Constants.h"
using namespace Entities;
using namespace Utils;

const float MarioTransition::STB_PART1_DURATION = 0.5f;
const float MarioTransition::STB_PART2_DURATION = 0.3f;
const float MarioTransition::SMOKE_DURATION = 0.4f;

MarioTransition::MarioTransition(LPMario player, PlayerPowerLevel to)
	: Entity(player->GetPosition(), "PlayerTransitions", GridType::NONE),
	player(player),
	state(EntityState<MarioTransition>(this)),
	time(0)
{

	player->Activate(false);

	animDirCode = player->GetFacingDirection() == HDirection::LEFT ? "Left" : "Right";
	PlayerPowerLevel from = player->GetPowerLevel();

	using PPL = ::PlayerPowerLevel;
	//small to big
	if (from == PPL::SMALL) {
		position.y -= Constants::TILE_SIZE;
		isDowngradeTransiton = false;
		resultPowerLevel = PlayerPowerLevel::BIG;
		state.SetState(&MarioTransition::SmallToBigPart1);
		SetAnimation("MarioTransitionPart1" + animDirCode);
		return;
	}

	//big to small
	if (from == PPL::BIG && to == PPL::SMALL) {
		//TODO: implement this.
		isDowngradeTransiton = true;
		return;
	}

	//upgrade 
	if (to != PPL::BIG) {
		isDowngradeTransiton = false;
		resultPowerLevel = to;
		state.SetState(&MarioTransition::Smoke);
		SetAnimation("MarioTransitionSmoke");
		return;
	}

	//downgrade
	else {
		isDowngradeTransiton = true;
		resultPowerLevel = to;
		state.SetState(&MarioTransition::Smoke);
		SetAnimation("MarioTransitionSmoke");
		return;
	}

}

void MarioTransition::OnReady()
{
	parentScene->GetCamera().StopFollowingEntity();
	parentScene->TransitionPause(true);
}

void MarioTransition::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);
}

void MarioTransition::SmallToBigPart1(float delta)
{
	time += delta;

	if (time >= STB_PART1_DURATION) {
		time = 0;
		SetAnimation("MarioTransitionPart2" + animDirCode);
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
	using PPL = ::PlayerPowerLevel;
	LPMario newPlayer = nullptr;
	//TODO: complete implementation
	switch (resultPowerLevel) {
	case PPL::SMALL:
		newPlayer = new MarioSmall(position, player->GetFacingDirection());
		break;

	case PPL::BIG:
		newPlayer = new MarioBig(position, player->GetFacingDirection());
		break;

	default:
		newPlayer = new MarioSmall(position, player->GetFacingDirection());
	}

	if (isDowngradeTransiton) {
		//TODO: implement this
	}

	newPlayer->SetVelocity(player->GetVelocity());

	parentScene->QueueFree(player);
	parentScene->AddEntity(newPlayer);
	parentScene->TransitionPause(false);
	parentScene->GetCamera().FollowEntity(newPlayer);
}

