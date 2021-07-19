#include "ProcessingUtils.h"

#include <fstream>
#include <sstream>

std::vector<std::string> ProcessingUtils::SplitByComma(const std::string& line) {
	std::vector<std::string> tokens;
	std::istringstream iss(line);

	for (std::string token; iss >> token;) {
		if (token[0] == '#')
			break;

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

std::vector<std::string> ProcessingUtils::SplitEvery(int splitLength, const std::string& line) {
	std::vector<std::string> tokens;
	for (size_t i = 0; i < line.size(); i += splitLength)
		tokens.push_back(line.substr(i, splitLength));

	return tokens;
}

std::string ProcessingUtils::GetNextNonCommentLine(std::ifstream& file)
{
	std::string resline = "EOF";
	std::string line;
	while (std::getline(file, line)) {
		if (line[0] != '#' && Trim(line) != "") {
			resline = line;
			break;
		}
	}

	return resline;
}

std::string ProcessingUtils::Trim(const std::string& str)
{
	int start = -1;
	int end = -1;
	for (size_t i = 0; i < str.size(); i++)
		if (str[i] != ' ') {
			start = i;
			break;
		}

	for (int i = static_cast<int>(str.size()) - 1; i >= 0; i--)
		if (str[i] != ' ') {
			start = i;
			break;
		}

	if (start == -1)
		return "";
	else
		return str.substr(start, (end - start) + 1);
}

std::string ProcessingUtils::JoinPath(const std::string& path1, const std::string& path2)
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

const char* ProcessingUtils::InvalidTokenSizeException::what() const throw () {
	return message;
}

ProcessingUtils::InvalidTokenSizeException::InvalidTokenSizeException(int numberOfArgsExpected) {
	std::string m = "Invalid token size: expected " + std::to_string(numberOfArgsExpected) + " params";
	message = m.c_str();
}

