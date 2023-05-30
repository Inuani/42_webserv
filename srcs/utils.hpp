#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "Settings.h"
#include <stdio.h>

std::string		    readFileContent(const std::string& path);
std::string		    getFileType(const std::string& filePath);
int				    getResponseCode(const std::string& filePath);
size_t			    getContentLen(const std::string& header);
const Location*	    findLocationByPath(const Settings& set, const std::string& path);
const std::string	repertoryListing(const std::string directoryPath);

#endif
