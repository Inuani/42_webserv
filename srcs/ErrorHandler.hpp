/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpouce <mpouce@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:47:14 by mpouce            #+#    #+#             */
/*   Updated: 2023/05/29 14:12:39 by mpouce           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include "Settings.h"

class ErrorHandler
{
private:
	int _intStatusCode;
	std::string _strStatusCode;
	std::map<std::string, std::string> _errorFileMap;
	std::string _body;
	Settings _settings;

	void readFile(const std::string& path);
	std::string getErrorFile() const;
public:
	ErrorHandler(const int statusCode, const std::map<std::string, std::string> errorFiles, const Settings& settings);
	~ErrorHandler();

	std::string getBody() const;
	void generateBody();
};
