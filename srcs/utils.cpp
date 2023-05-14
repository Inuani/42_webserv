#include <fstream>
#include <sstream>
#include "utils.hpp"
#include <iostream>

std::string readFileContent(const std::string& path) {
	std::ifstream file(path, std::ios::binary);

	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << path << '\n';
	}
	std::ostringstream ss;

	ss << file.rdbuf();
	return ss.str();
}
