#pragma once
#include <windows.h>
#include <d3dx9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "GameSettings.h"
#include "Scene.h"

class Game
{
public:
	static void Init(HWND hWnd, const GameSettings& gameSettings);

	static void Run();
	static const GameSettings& GetGameSettings();
	static bool IsKeyDown(int KeyCode);
	static bool IsKeyPressed(int keyCode);
	static void EnableCollisionEngine(bool state);
	static LPDIRECT3DDEVICE9 GetDirect3DDevice();
	static void SwitchScene(LPScene scene);

	/// <summary>
	/// Queue free previous scene and switch to new scene
	/// </summary>
	static void QueueFreeAndSwitchScene(std::string scenePath);

	/// <summary>
	/// Get scene that is currently active (running in the game loop)
	/// </summary>
	static LPScene GetActiveScene();

	/// <summary>
	/// Begin rendering process, Must be called before performing any rendering (e.g: Game::Draw calls)
	/// </summary>
	static HRESULT BeginRender();

	/// <summary>
	/// End rendering process, Must be called before calling BeginRender
	/// </summary>
	static HRESULT EndRender();

	/// <summary>
	/// Draw out an area of a texture
	/// </summary>
	static void Draw(LPDIRECT3DTEXTURE9 texure, const RECT& boundingBox, const Utils::Vector2<float>& position);

	/// <summary>
	/// Fill backbuffer with color
	/// </summary>
	/// <param name="color"></param>
	static HRESULT ColorFill(const D3DCOLOR& color);

	/// <summary>
	/// Display back buffer content to the screen
	/// </summary>
	static HRESULT Present();


private:
	static void Release();
	static void InitAndSwitchScene();
	static Utils::Vector2<float> ToPixelPerfectPosition(const Utils::Vector2<float>& position);
	static Utils::Vector2<float> ToPrecisePosition(const Utils::Vector2<float>& position);
	static void ProcessKeyboard();
	static LPDIRECT3DDEVICE9 CreateDirect3DDevice(LPDIRECT3D9 d3d, HWND windowHandle);
	static LPDIRECTINPUTDEVICE8 CreateDirectInputDevice(LPDIRECTINPUT8 di, HWND windowHandle, DWORD keyboardBufferSize);

	static GameSettings gameSettings;
	static bool collisionEngineEnabled;

	static HWND windowHandle;
	static D3DPRESENT_PARAMETERS d3dpp;
	static LPDIRECT3D9 d3d;
	static LPDIRECT3DDEVICE9 d3ddv;
	static LPDIRECTINPUTDEVICE8 didv;
	static LPDIRECTINPUT8 di;
	static LPD3DXSPRITE d3dxSprite;

	static const int KEYBOARD_BUFER_SIZE;
	static BYTE keyStates[];
	static DIDEVICEOBJECTDATA keyEvents[];
	static DWORD dwInOut;

	static LPDIRECT3DSURFACE9 backBuffer;

	static LPScene activeScene;
	static LPScene newActiveScene;
	static LPScene waitForDeletionScene;

};

