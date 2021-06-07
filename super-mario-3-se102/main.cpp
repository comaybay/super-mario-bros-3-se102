#include <fstream>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>

#include "Game.h"
#include "Utils.h"
#include "ResourceLoader.h"


int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_  HINSTANCE hInstPrev, _In_ PSTR cmdline, _In_ int cmdshow)
{
	//standard display resolution of NES console
	const Utils::Dimension dim = Utils::Dimension(256, 240);
	const int scale = 3;

	HWND gameWindowHandler = Utils::CreateGameWindow(hInst, cmdshow, dim.width * scale, dim.height * scale);
	Game::Init(gameWindowHandler, scale, "data", dim);
	Game::Run();
	Game::Release();
	return 0;
}
