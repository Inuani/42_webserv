#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string readFileContent(const std::string& path);
std::string getFileType(const std::string& filePath);
int getResponseCode(const std::string& filePath);
int getContentLen(const std::string& header);

#endif
