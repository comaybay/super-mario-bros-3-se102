#include "Utils.h"
#include <windows.h>

void Utils::DebugOut(std::string text) {
	std::string msg = text + "\n";
	OutputDebugStringA(msg.c_str());
}
void Utils::DebugOut(const char* text) {
	std::string msg = text + std::string("\n");
	OutputDebugStringA(text);
}

float Utils::Deg2Rad(float deg)
{
	return deg / 180 * Constants::PI;
}

float Utils::Rad2Deg(float rad)
{
	return rad / Constants::PI * 180;
}

bool Utils::AlmostEqual(float a, float b)
{
	return abs(a - b) < std::numeric_limits<float>::epsilon();
}

float Utils::AngleToXAxis(const Vector2<float>& vector, const Vector2<float>& origin) {
	return Rad2Deg(atan2f(origin.y - vector.y, vector.x - origin.x));
}
