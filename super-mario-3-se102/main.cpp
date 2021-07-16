#include <fstream>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>

#include "Game.h"
#include "Utils.h"
#include "ProcessingUtils.h"
using namespace ProcessingUtils;

GameSettings ParseGameSettings();
HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, const Utils::Dimension<int>& screenDim);
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_  HINSTANCE hInstPrev, _In_ PSTR cmdline, _In_ int cmdshow)
{
	GameSettings gameSettings = ParseGameSettings();
	HWND gameWindowHandler = CreateGameWindow(hInst, cmdshow, gameSettings.gameDimension * gameSettings.pixelScale);

	Game::Init(gameWindowHandler, gameSettings);
	Game::Run();

	return 0;
}

GameSettings ParseGameSettings()
{
	std::ifstream file("data/config.txt");

	std::string line = "";
	while (line != "EOF") {
		line = GetNextNonCommentLine(file);
		if (line[0] != '[' || line != "[GAME SETTINGS]")
			continue;

		std::vector<std::string> dimTokens = SplitByComma(GetNextNonCommentLine(file));
		if (dimTokens.size() != 2)
			throw InvalidTokenSizeException(2);

		Utils::Dimension<int> gameDimension(stoi(dimTokens[0]), stoi(dimTokens[1]));

		std::vector<std::string> scaleToken = SplitByComma(GetNextNonCommentLine(file));
		if (scaleToken.size() != 1)
			throw InvalidTokenSizeException(1);

		int pixelScale = stoi(scaleToken[0]);

		std::vector<std::string> fpsToken = SplitByComma(GetNextNonCommentLine(file));
		if (fpsToken.size() != 1)
			throw InvalidTokenSizeException(1);

		int maxFPS = stoi(fpsToken[0]);

		std::vector<std::string> pprToken = SplitByComma(GetNextNonCommentLine(file));
		if (pprToken.size() != 1)
			throw InvalidTokenSizeException(1);

		bool pixelPerfectRendering = (pprToken[0] == "True") ? true : false;

		std::vector<std::string> dirToken = SplitByComma(GetNextNonCommentLine(file));
		if (dirToken.size() != 1)
			throw InvalidTokenSizeException(1);

		const std::string& dataDirectory = dirToken[0];

		std::vector<std::string> initalSceneToken = SplitByComma(GetNextNonCommentLine(file));
		if (initalSceneToken.size() != 1)
			throw InvalidTokenSizeException(1);

		const std::string& initialScenePath = initalSceneToken[0];

		file.close();
		return GameSettings(gameDimension, pixelScale, maxFPS, pixelPerfectRendering, dataDirectory, initialScenePath);
	}

	file.close();
	throw std::exception("GetGameSettings Failed: missing [GAME SETTINGS] section");
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, const Utils::Dimension<int>& screenDim)
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Game Screen";
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	//CreateWindow() nWidth and nHeight take into account window's title bar and borders
	//use AdjustWindowRect to calculate the overall Dimension<float>:
	//taken from https://stackoverflow.com/questions/11783086/get-exact-window-region-size-createwindow-window-size-isnt-correct-size-of-wi
	RECT rect{ 0, 0, screenDim.width, screenDim.height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	HWND hWnd =
		CreateWindow(
			wc.lpszClassName,
			"SMB3 Mock",
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			0,
			0,
			rect.right - rect.left - 1, //for some reason you have to -1 
			rect.bottom - rect.top - 1,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd)
		throw "Failed to create game window";

	//center window
	//taken from: https://forums.codeguru.com/showthread.php?500841-Set-my-window-position-at-screen-center
	RECT rc;
	GetWindowRect(hWnd, &rc);
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;
	SetWindowPos(hWnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
