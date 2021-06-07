#include "Utils.h"
#include <stringapiset.h>
#include <sstream>
#include <d3dx9.h>
using namespace Utils;

void Utils::DebugOut(std::string text) {
	std::string msg = text + "\n";
	OutputDebugStringA(msg.c_str());
}
void Utils::DebugOut(const char* text) {
	std::string msg = text + std::string("\n");
	OutputDebugStringA(text);
}

Dimension::Dimension() {};

Dimension::Dimension(float width, float height) : width(width), height(height) {};

Dimension Dimension::Rounded() {
	return Dimension(round(width), round(height));
}

Vector2::Vector2() {};

Vector2::Vector2(float x, float y) : x(x), y(y) {};

Vector2 Vector2::Rounded() {
	return Vector2(round(x), round(y));
}

float Utils::Vector2::DistanceTo(const Vector2& v)
{
	return sqrt(pow(x - v.x, 2) + pow(y - v.y, 2));
}

Vector2 Vector2::operator+(const Utils::Vector2& other) const {
	return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
	return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator- () const {
	return  Vector2(-x, -y);
}

Vector2& Vector2::operator+=(const Vector2& other) {
	x += other.x;
	y += other.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& other) {
	x -= other.x;
	y -= other.y;
	return *this;
}

Vector2 Vector2::operator*(const Vector2& other) const {
	return Vector2(x * other.x, y * other.y);
}

Vector2 Vector2::operator*(int value) const {
	return Vector2(x * value, y * value);
}

Vector2 Vector2::operator*(float value) const {
	return Vector2(x * value, y * value);
}


//from: https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
std::wstring Utils::StringToWideString(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::vector<std::string> Utils::SplitByComma(const std::string& line) {
	std::vector<std::string> tokens;
	std::istringstream iss(line);

	for (std::string token; iss >> token;) {
		if (token == ",")
			continue;

		size_t index = token.find(',', 0);
		if (index == std::string::npos) {
			tokens.push_back(token);
		}
		else if (index == token.size() - 1) {
			tokens.push_back(token.substr(0, token.size() - 1));
		}
		else if (index == 0) {
			tokens.push_back(token.substr(1, token.size()));
		}
		else {
			tokens.push_back(token.substr(0, index));
			tokens.push_back(token.substr(index + 1, token.size() - 1));
		}
	}

	return tokens;
}

std::vector<std::string> Utils::SplitEvery(int splitLength, const std::string& line) {
	std::vector<std::string> tokens;
	for (size_t i = 0; i < line.size(); i += splitLength)
		tokens.push_back(line.substr(i, splitLength));

	return tokens;
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

HWND Utils::CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int screenWidth, int screenHeight)
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


LPDIRECT3DDEVICE9 Utils::CreateDirect3DDevice(LPDIRECT3D9 d3d, HWND windowHandle) {
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

LPDIRECTINPUTDEVICE8 Utils::CreateDirectInputDevice(LPDIRECTINPUT8 di, HWND hWnd, DWORD keyboardBufferSize) {

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


std::vector<Utils::SpriteBox> Utils::CreateSpriteBoxSequence(Vector2 startPosition, Dimension dimension, int space, int frameCount, Vector2 offset)
{
	std::vector<Utils::SpriteBox> sequence;
	for (int i = 0; i < frameCount; i++) {
		Vector2 position(
			(int)startPosition.x + (dimension.width + space) * i,
			(int)startPosition.y
		);

		sequence.emplace_back
		(
			RECT{
				(int)position.x,
				(int)position.y,
				(int)(position.x + dimension.width),
				(int)(position.y + dimension.height),
			},
			offset
			);
	}
	return sequence;
}

int Utils::Sign(int number)
{
	return (number > 0) - (number < 0);
}

std::string Utils::JoinPath(const std::string& path1, const std::string& path2)
{
	if (path1[path1.size() - 1] == '/') {
		if (path2[0] == '/')
			return path1.substr(0, path1.size() - 1) + path2;
		else
			return path1 + path2;
	}

	if (path2[0] == '/')
		return path1 + path2;

	return path1 + "/" + path2;
}

int Utils::HexCharToInt(const char& ch) {
	if (ch >= '0' && ch <= '9')
		return ch - '0';

	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;

	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;

	throw std::exception("charToInt failed: invaild hex char");
}

float Utils::Sign(float number)
{
	return (number > 0) - (number < 0);
}


const char* Utils::InvalidTokenSizeException::what() const throw () {
	return message;
}

Utils::InvalidTokenSizeException::InvalidTokenSizeException(int numberOfArgsExpected) {
	std::string m = "Invalid token size: expected " + std::to_string(numberOfArgsExpected) + " params";
	message = m.c_str();
}

