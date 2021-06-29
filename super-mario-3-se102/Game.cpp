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
#include "SceneLoader.h"

std::string Game::dataDir;
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
LPScene Game::activeScene = nullptr;
LPScene Game::newActiveScene = nullptr;
LPScene Game::waitForDeletionScene = nullptr;
bool Game::enableCollisionEngine = true;

void Game::Init(HWND hWnd, float scale, std::string dataDirectory, Utils::Dimension gameDim)
{
	Game::dataDir = dataDirectory;
	Game::windowHandle = hWnd;
	Game::scale = scale;
	Game::gameDim = gameDim;

	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	d3ddv = CreateDirect3DDevice(d3d, hWnd);

	d3ddv->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	DirectInput8Create
	(
		//WARNING: GWL_HINSTANCE is undefined if your solution platform is x64, switch to x86 instead
		//from: https://stackoverflow.com/questions/51771072/gwl-wndproc-undeclared/51772471
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&di, NULL
	);

	didv = CreateDirectInputDevice(di, hWnd, KEYBOARD_BUFER_SIZE);
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

	activeScene = SceneLoader::LoadScene("data/world_maps/wm_1.txt");
	//activeScene = SceneLoader::LoadScene("data/worlds/w_1_1_1.txt");
	CollisionEngine::_SetActiveCED(activeScene);
}

LPDIRECT3DDEVICE9 Game::CreateDirect3DDevice(LPDIRECT3D9 d3d, HWND windowHandle) {
	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;

	// retrieve WindowClient width & height to set back buffer width & height accordingly
	RECT r;
	GetClientRect(windowHandle, &r);

	d3dpp.BackBufferHeight = r.bottom + 1;
	d3dpp.BackBufferWidth = r.right + 1;

	int backBufferWidth = d3dpp.BackBufferWidth;
	int backBufferHeight = d3dpp.BackBufferHeight;

	LPDIRECT3DDEVICE9 d3ddv;
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		windowHandle,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, //https://docs.microsoft.com/vi-vn/windows/win32/direct3d9/d3dcreate?redirectedfrom=MSDN
		&d3dpp,
		&d3ddv);

	if (d3ddv == NULL)
		throw std::exception("Create Direct3DDevice failed");

	d3ddv->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);

	return d3ddv;
}

LPDIRECTINPUTDEVICE8 Game::CreateDirectInputDevice(LPDIRECTINPUT8 di, HWND hWnd, DWORD keyboardBufferSize) {

	LPDIRECTINPUTDEVICE8 didv;
	di->CreateDevice(GUID_SysKeyboard, &didv, NULL);

	// Set the data format to "keyboard format" - a predefined data format 
	//
	// A data format specifies which controls on a device we
	// are interested in, and how they should be reported.
	//
	// This tells DirectInput that we will be passing an array
	// of 256 bytes to IDirectInputDevice::GetDeviceState.

	didv->SetDataFormat(&c_dfDIKeyboard);
	didv->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);


	// IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
	//
	// DirectInput uses unbuffered I/O (buffer size = 0) by default.
	// If you want to read buffered data, you need to set a nonzero
	// buffer size.
	//
	// Set the buffer size to DINPUT_BUFFERSIZE (defined above) elements.
	//
	// The buffer size is a DWORD property associated with the device.
	DIPROPDWORD dipdw{};
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = keyboardBufferSize; // Arbitary buffer size

	didv->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	return didv;
}

void Game::EnableCollisionEngine(bool state) {
	enableCollisionEngine = state;
}

void Game::SwitchScene(LPScene scene) {
	newActiveScene = scene;
}

void Game::QueueFreeAndSwitchScene(LPScene scene) {
	waitForDeletionScene = activeScene;
	newActiveScene = scene;
}

void Game::QueueFreeAndSwitchScene(std::string scenePath)
{
	LPScene newScene = SceneLoader::LoadScene(scenePath);
	QueueFreeAndSwitchScene(newScene);
}

int Game::GetScale()
{
	return scale;
}

const Utils::Dimension& Game::GetGameDimension()
{
	return gameDim;
}

const LPScene Game::GetActiveScene()
{
	return activeScene;
}

const std::string& Game::GetDataDirectory()
{
	return dataDir;
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
			if (enableCollisionEngine)
				CollisionEngine::Update(dt);

			activeScene->Update(dt);
			accumulator -= frameTime;

			CollisionEngine::HandleUnsubscribeWaitList();
		}

		activeScene->Render();

		if (newActiveScene != nullptr) {
			activeScene = newActiveScene;
			CollisionEngine::_SetActiveCED(activeScene);
			newActiveScene = nullptr;

			if (waitForDeletionScene != nullptr) {
				LPScene scene = waitForDeletionScene;
				delete waitForDeletionScene;
				//entities need to be destroyed before event
				CollisionEngine::_RemoveCED(scene);
				waitForDeletionScene = nullptr;
			}
		}
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

