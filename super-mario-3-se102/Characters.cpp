#include "Characters.h"
#include "Game.h"
using namespace Characters;
using namespace Utils;

Mario::Mario(Vector2 position) :
	Entity::Entity(position, AnimationManager::Get("MarioSIR"))
{
	maxSpeed = Vector2(80, 100);
	acceleration = Vector2(180, 0);
}

void Mario::Update(float delta)
{
	Entity::Update(delta);
	if (Game::IsKeyDown(DIK_RIGHT)) {
		prevPressedKey = DIK_RIGHT;
		dir.x = 1;
		SetAnimation("MarioSMR");
	}
	else if (Game::IsKeyDown(DIK_LEFT)) {
		prevPressedKey = DIK_LEFT;
		SetAnimation("MarioSML");
		dir.x = -1;
	}
	else {
		dir.x = -Utils::Sign(velocity.x);

		if (abs(velocity.x) > acceleration.x * delta)
			velocity.x += acceleration.x * dir.x * delta;
		else {
			velocity.x = 0;
			if (prevPressedKey == DIK_LEFT) SetAnimation("MarioSIL");
			else if (prevPressedKey == DIK_RIGHT) SetAnimation("MarioSIR");
		}

		position.x += velocity.x * delta;
		return;
	}

	position += velocity * delta;

	velocity += acceleration * dir * delta;
	velocity.x = (dir.x < 0) ? max(velocity.x, -maxSpeed.x) : min(velocity.x, maxSpeed.x);
	velocity.y = (dir.y < 0) ? max(velocity.y, -maxSpeed.y) : min(velocity.y, maxSpeed.y);
}
