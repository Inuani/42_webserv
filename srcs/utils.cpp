#include <fstream>
#include <sstream>
#include "utils.hpp"

std::string readFileContent(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	std::ostringstream ss;

	ss << file.rdbuf();
	return ss.str();
}