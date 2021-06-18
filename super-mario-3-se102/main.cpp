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
	const std::string dataDirectory = "data";
	Utils::Dimension gameDim;
	int scale;
	ResourceLoader(dataDirectory).GetGameSettings(gameDim, scale);
	HWND gameWindowHandler = Utils::CreateGameWindow(hInst, cmdshow, gameDim.width * scale, gameDim.height * scale);

	Game::Init(gameWindowHandler, scale, dataDirectory, gameDim);
	Game::Run();

	return 0;
}
