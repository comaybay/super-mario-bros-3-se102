#include "Color.h"

std::string Color::GREEN = "Green";
std::string Color::RED = "Red";
std::string Color::BROWN = "Brown";

std::string Color::ToColorCode(std::string color)
{
	if (color == GREEN)
		return "G";
	else if (color == RED)
		return "R";
	else if (color == BROWN)
		return "B";
}
