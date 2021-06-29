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

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight);
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_  HINSTANCE hInstPrev, _In_ PSTR cmdline, _In_ int cmdshow)
{
	Utils::Dimension gameDim;
	int scale;
	int maxFPS;
	std::string dataDir;
	ResourceLoader("data").GetGameSettings(gameDim, scale, maxFPS, dataDir);
	HWND gameWindowHandler = CreateGameWindow(hInst, cmdshow, gameDim.width * scale, gameDim.height * scale);

	Game::Init(gameWindowHandler, gameDim, scale, maxFPS, dataDir);
	Game::Run();

	return 0;
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int screenWidth, int screenHeight)
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
	//use AdjustWindowRect to calculate the overall dimension:
	//taken from https://stackoverflow.com/questions/11783086/get-exact-window-region-size-createwindow-window-size-isnt-correct-size-of-wi
	RECT rect{ 0, 0, screenWidth, screenHeight };
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
