#include "Colors.h"

std::string Colors::GREEN = "Green";
std::string Colors::RED = "Red";
std::string Colors::BROWN = "Brown";

std::string Colors::ToColorCode(std::string color)
{
	if (color == GREEN)
		return "G";
	else if (color == RED)
		return "R";
	else if (color == BROWN)
		return "B";
}
