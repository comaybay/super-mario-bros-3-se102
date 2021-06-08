#include "Game.h"
#include "Utils.h"
#include "TextureManager.h"
#include "AnimationManager.h"
#include "EntityManager.h"
#include "Entities.h"
#include "CollisionEngine.h"
#include <sstream>
#include <thread>
#include <chrono>
#include "ResourceLoader.h"
#include "SceneManager.h"

HWND Game::windowHandle;
Utils::Dimension Game::gameDim;
int Game::scale;
D3DXMATRIX Game::scaleMatrix;
LPDIRECT3D9 Game::d3d;
LPDIRECT3DDEVICE9 Game::d3ddv;
LPDIRECTINPUT8 Game::di;
LPDIRECTINPUTDEVICE8 Game::didv;
LPDIRECT3DSURFACE9 Game::backBuffer;
LPD3DXSPRITE Game::d3dxSprite;
BYTE Game::keyStates[256];
DIDEVICEOBJECTDATA Game::keyEvents[Game::KEYBOARD_BUFER_SIZE];
DWORD Game::dwInOut = Game::KEYBOARD_BUFER_SIZE;
const Utils::Vector2 Game::Gravity = Utils::Vector2(0, 1200);
LPScene Game::scene = nullptr;


void Game::Init(HWND hWnd, float scale, std::string dataDirectory, Utils::Dimension gameDim)
{
	Game::windowHandle = hWnd;
	Game::scale = scale;
	Game::gameDim = gameDim;

	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	d3ddv = Utils::CreateDirect3DDevice(d3d, hWnd);

	d3ddv->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	DirectInput8Create
	(
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&di, NULL
	);

	didv = Utils::CreateDirectInputDevice(di, hWnd, KEYBOARD_BUFER_SIZE);
	didv->Acquire();

	//used for rendering
	Game::scaleMatrix = {
		scale,           0.0f,            0.0f,            0.0f,
		0.0f,            scale,           0.0f,            0.0f,
		0.0f,            0.0f,            scale,           0.0f,
		0.0f,            0.0f,            0.0f,            1.0f
	};
	D3DXCreateSprite(d3ddv, &d3dxSprite);
	d3dxSprite->SetTransform(&scaleMatrix);

	ResourceLoader(dataDirectory).Load();

	//TODO: remove test code
	SceneManager::AddScenePath("data/worlds/w_1_1_1.txt", "World 1-1-1");

	scene = SceneManager::LoadWorld("World 1-1-1");
}

int Game::GetScale()
{
	return scale;
}

Utils::Dimension Game::GetGameDimension()
{
	return gameDim;
}

const LPScene Game::GetScene()
{
	return scene;
}

const LPEntityManager Game::GetSceneEntityManager()
{
	return scene->GetEntityManager();
}

void Game::Run()
{
	MSG msg;
	ULONGLONG prev = GetTickCount64();	//unit is in ms
	int frameTime = round(1000.0f / 60.0f);
	const float dt = 1.0f / 60.0f;
	int accumulator = 0;

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				Game::Release();
				return;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ULONGLONG now = GetTickCount64();
		ULONGLONG duration = now - prev;
		prev = now;

		accumulator += duration;

		ProcessKeyboard();
		if (accumulator >= frameTime)
		{
			scene->Update(dt);
			accumulator -= frameTime;
		}

		scene->Render();
		Game::GetSceneEntityManager()->FreeEntitiesInQueue();
	}
}

void Game::ProcessKeyboard()
{
	HRESULT hr;

	// Collect all key states first
	hr = didv->GetDeviceState(sizeof(keyStates), keyStates);
	// If the keyboard lost focus or was not acquired then try to get control back.
	if (FAILED(hr) && (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED))
		didv->Acquire();

	// Collect all buffered events
	dwInOut = KEYBOARD_BUFER_SIZE;
	didv->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), keyEvents, &dwInOut, 0);

	//if (dwInOut == KEYBOARD_BUFER_SIZE)
	//	dwInOut == 0;

	// Scan through all buffered events, check if the key is pressed or released
}

bool Game::IsKeyDown(int keyCode) {
	return (keyStates[keyCode] & 0x80) > 0;
}

bool Game::IsKeyPressed(int keyCode) {
	// Scan through all buffered events, check if the key is pressed or released
	for (DWORD i = 0; i < dwInOut; i++)
	{
		int thisKeyCode = keyEvents[i].dwOfs;
		int thisKeyState = keyEvents[i].dwData;
		if ((thisKeyCode == keyCode) && (thisKeyState & 0x80) > 0)
			return true;
	}
	return false;
}

void Game::Release()
{
	d3dxSprite->Release();
	backBuffer->Release();
	d3ddv->Release();
	d3d->Release();
}

