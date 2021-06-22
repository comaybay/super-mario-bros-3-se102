#include "Contains.h"

bool Utils::Contains(const char* elem, const std::vector<std::string>& vector) {
	return (std::find(vector.begin(), vector.end(), std::string(elem)) != vector.end());
}

