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

Dimension::Dimension() {};

Dimension::Dimension(float width, float height) : width(width), height(height) {}

Dimension Dimension::operator/(float value) const {
	return Dimension(width / value, height / value);
}

Dimension Dimension::operator*(float value) const {
	return Dimension(width * value, height * value);
}

Dimension Dimension::operator*(int value) const {
	return Dimension(width * value, height * value);
}

Dimension Dimension::operator+(const Dimension& other) const {
	return Dimension(width + other.width, height + other.height);
}

Dimension Dimension::Rounded() {
	return Dimension(round(width), round(height));
}

