#include "Utils.h"
#include <windows.h>

using namespace Utils;

void Utils::DebugOut(std::string text) {
	std::string msg = text + "\n";
	OutputDebugStringA(msg.c_str());
}
void Utils::DebugOut(const char* text) {
	std::string msg = text + std::string("\n");
	OutputDebugStringA(text);
}

