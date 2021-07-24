#pragma once
#include "TitleScene.h"
#include "Game.h"
#include "AnimationManager.h"
#include "EntityManager.h"


TitleScene::TitleScene() :
	titleScreenAnim(AnimationManager::GetNew("TitleScreen"))
{
	entityManager = new EntityManager(new Grid(0, 0, { 0, 0 }), new Grid(0, 0, { 0, 0 }), new DynamicGrid(0, 0, { 0, 0 }));
}

void TitleScene::_Ready()
{
}

void TitleScene::Update(float delta)
{
	if (Game::IsKeyPressed(DIK_W) || Game::IsKeyPressed(DIK_A) || Game::IsKeyPressed(DIK_S))
		Game::QueueFreeAndSwitchScene(Game::GetGameSettings().initialScenePath);
}

void TitleScene::Render()
{
	if (Game::BeginRender() == S_OK) {
		Game::DrawHUD(titleScreenAnim->GetTexture(), titleScreenAnim->GetCurrentSpriteBox().rect, { 0, 0 });
		Game::EndRender();
	}

	Game::Present();
}
