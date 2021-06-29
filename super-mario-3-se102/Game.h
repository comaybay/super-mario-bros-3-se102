#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <windows.h>
#include <d3dx9.h>
#include <dinput.h>
#include "Utils.h"
#include "Scene.h"

class Game
{
public:
	static void Init(HWND windowHandle, float scale, std::string dataDirectory, Utils::Dimension gameDim);
	static LPDIRECT3D9 GetD3D9() { return d3d; };
	static LPDIRECT3DDEVICE9 GetDirect3DDevice() { return d3ddv; };
	static LPDIRECT3DSURFACE9 GetBackBuffer() { return backBuffer; };
	static LPD3DXSPRITE GetD3DXSprite() { return d3dxSprite; };
	static int GetScale();
	static const Utils::Dimension& GetGameDimension();
	static const LPScene GetActiveScene();
	static const std::string& GetDataDirectory();
	static void Run();
	static void Release();
	static void ProcessKeyboard();
	static bool IsKeyDown(int KeyCode);
	static bool IsKeyPressed(int keyCode);
	static void SwitchScene(LPScene scene);
	static void EnableCollisionEngine(bool state);
	static const int TILE_SIZE = 16;
private:
	static LPDIRECT3DDEVICE9 CreateDirect3DDevice(LPDIRECT3D9 d3d, HWND windowHandle);
	static LPDIRECTINPUTDEVICE8 CreateDirectInputDevice(LPDIRECTINPUT8 di, HWND windowHandle, DWORD keyboardBufferSize);
	static std::string dataDir;
	static int scale;
	static Utils::Dimension gameDim;
	static D3DXMATRIX scaleMatrix;
	static HWND windowHandle;
	static LPDIRECT3D9 d3d;
	static LPDIRECT3DDEVICE9 d3ddv;
	static LPDIRECTINPUT8 di;
	static LPDIRECTINPUTDEVICE8 didv;
	static const int KEYBOARD_BUFER_SIZE = 1024;
	static BYTE keyStates[256];
	static DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFER_SIZE];
	static DWORD dwInOut;
	static LPDIRECT3DSURFACE9 backBuffer;
	static LPD3DXSPRITE d3dxSprite;
	static LPScene activeScene;
	static bool enableCollisionEngine;
};

