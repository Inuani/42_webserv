#include "ReqHandler.hpp"
#include <iostream>
#include "utils.hpp"
#include <sstream>
#include <map>
#include <fstream>
#include <stdio.h>

ReqHandler::ReqHandler() {}

const std::string	ReqHandler::_getReqHandler(const HttpReqParsing& request) {
	//std::string filePath = request.getUri();
	std::string filePath = "www/html";
	std::string defaultFile = "/index.html";
	if (request.getUri() == "/")
		filePath.append(defaultFile);
	else
		filePath.append(request.getUri());

	std::string body = readFileContent(filePath);
	HttpResponse hRes(getResponseCode(filePath), body, getFileType(filePath));
	std::string response = hRes.toString();
	// std::cout << response << std::endl;
	return response;
}

const std::string	ReqHandler::_postReqHandler(const HttpReqParsing& request) {
	std::map<std::string, std::string> headers = request.getheaders();
	std::string contentType = headers["Content-Type"];

	std::cout << contentType << std::endl;
	// std::cout << request.getBody() << std::endl;

	if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
		std::string formData = request.getBody();
		std::map<std::string, std::string> formFields;
		std::istringstream formStream(formData);
		std::string field;
		while (std::getline(formStream, field, '&')) {
			std::string key = field.substr(0, field.find('='));
			std::string value = field.substr(field.find('=') + 1);
			formFields[key] = value;
		}
		std::ofstream outFile("./data/form_submissions.txt", std::ios_base::app);
		for (std::map<std::string, std::string>::const_iterator it = formFields.begin(); it != formFields.end(); ++it) {
			outFile << it->first << ": " << it->second << "\n";
		}
		outFile.close();
		HttpResponse hRes(303, "", "text/html");
		hRes.setLocationHeader("/success.html");
		return hRes.toString();
	}
	else if (contentType.find("multipart/form-data") != std::string::npos) {

		//std::string boundary = request.getBody().substr(0, 40); //caution
		std::string boundary = contentType.substr(contentType.find("boundary=") + 9, contentType.size() - 10 - contentType.find("boundary="));
		std::string content = request.getBody().substr(request.getBody().find("\r\n\r\n") + 4, std::string::npos);
		content.erase(content.find(boundary), std::string::npos);

		std::string filename = request.getBody().substr(request.getBody().find("filename=") + 10, std::string::npos);
		filename.erase(filename.find("\""), std::string::npos);
		std::string directoryPath = "./data/";
		filename = directoryPath + filename;
		std::ofstream file(filename);
		file << content;

		HttpResponse hRes(303, "", "text/html");
		hRes.setLocationHeader("/uploadSuccess.html");
		return hRes.toString();
	}
	HttpResponse hRes(404, "Error", "text/html");
	return hRes.toString();
}

const std::string	ReqHandler::_deleteReqHandler(const HttpReqParsing& request) {
	if (request.getUri() == "/delete_submissions") {
		if (remove("./data/form_submissions.txt") != 0) {
			HttpResponse hRes(500, "Error deleting file", "text/plain");
			return hRes.toString();
		}
		HttpResponse hRes(303, "", "text/html"); //not sure it is working
		hRes.setLocationHeader("/success.html");
		return hRes.toString();
	}
	else if (request.getUri() == "/delete_all") {
		std::system("rm -rf ./data/*");
		HttpResponse hRes(303, "", "text/html"); //not sure it is working either !
		hRes.setLocationHeader("/success.html");
		return hRes.toString();
	}
	HttpResponse hRes(404, "Error", "text/plain");
	return hRes.toString();
}

const std::string	ReqHandler::_defaultHandler(const HttpReqParsing& request) {
	HttpResponse hRes(404, "Error", "text/plain");
	std::string response = hRes.toString();
	return response;
}

const std::string	ReqHandler::_phpCgiHandler(const HttpReqParsing& request) {
	
	
	std::string response = "";
	return response;
}

const std::string	ReqHandler::handleRequest(const HttpReqParsing& request) {
	std::string filePath = request.getUri();
	if (filePath.find_last_of(".") != std::string::npos && filePath.substr(filePath.find_last_of(".") + 1) == "php") {
		return _phpCgiHandler(request);
	}
	else if (request.getMethod() == "GET") {
		return _getReqHandler(request);
	} else if (request.getMethod() == "POST") {
		return _postReqHandler(request);
	} else if (request.getMethod() == "DELETE") {
		return _deleteReqHandler(request);
	} else {
		return _defaultHandler(request);
	}
}

ReqHandler::ReqHandler(const ReqHandler &src) {
	*this = src;
}

ReqHandler::~ReqHandler() {}

ReqHandler&	ReqHandler::operator=(const ReqHandler& rhs) {
	(void)rhs;
	return *this;
}
