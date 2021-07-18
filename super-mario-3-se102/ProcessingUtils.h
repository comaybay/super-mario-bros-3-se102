#pragma once
#include <string>
#include <vector>

namespace ProcessingUtils {
	std::string GetNextNonCommentLine(std::ifstream& file);
	
	std::string Trim(const std::string& str);

	std::vector<std::string> SplitByComma(const std::string& line);

	std::vector<std::string> SplitEvery(int splitLength, const std::string& line);

	std::string JoinPath(const std::string& path1, const std::string& path2);

	class InvalidTokenSizeException : public std::exception {
	public:
		InvalidTokenSizeException(int numberOfArgsExpected);
		const char* what() const throw ();

	private:
		const char* message;
	};
}