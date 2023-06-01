/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpouce <mpouce@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:47:14 by mpouce            #+#    #+#             */
/*   Updated: 2023/06/01 16:15:19 by mpouce           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include "Settings.h"
#include "utils.hpp"

class ErrorHandler
{
private:
	int _intStatusCode;
	std::string _strStatusCode;
	std::map<std::string, std::string> _errorFileMap;
	std::string _body;
	Settings _settings;

	std::string _filename;
	std::string _filedir;

	void readFile(const std::string& path);
	std::string getErrorFile();
	std::string findFileLocation(std::string filePath);

	ErrorHandler();
	ErrorHandler(const ErrorHandler& ref);
	ErrorHandler& operator=(const ErrorHandler& ref);
public:
	ErrorHandler(const int statusCode, const std::map<std::string, std::string> errorFiles, const Settings& settings);
	~ErrorHandler();

	std::string getBody() const;
	void generateBody();
};
