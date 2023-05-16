#include "ReqHandler.hpp"
#include <iostream>
#include "utils.hpp"
#include <sstream>
#include <map>
#include <fstream>

ReqHandler::ReqHandler() {}

const std::string	ReqHandler::getReqHandler(const HttpReqParsing& request) {
	std::string filePath = request.getUri();
	if (request.getUri() == "/")
		filePath = "srcs/index.html";
	else if (request.getUri() == "/success.html")
		filePath = "srcs/success.html";
	else if (request.getUri() == "/submit")
		filePath = "srcs/success.html";
	std::string body = readFileContent(filePath);
	HttpResponse hRes(200, body, "text/html");
	std::string response = hRes.toString();
	// std::cout << response << std::endl;
	return response;
}

const std::string	ReqHandler::postReqHandler(const HttpReqParsing& request) {
	std::string formData = request.getBody();
	std::map<std::string, std::string> formFields;
	std::istringstream formStream(formData);
	std::string field;
	while (std::getline(formStream, field, '&')) {
		std::string key = field.substr(0, field.find('='));
		std::string value = field.substr(field.find('=') + 1);
		formFields[key] = value;
	}

	std::ofstream outFile("form_submissions.txt", std::ios_base::app);
	for (std::map<std::string, std::string>::const_iterator it = formFields.begin(); it != formFields.end(); ++it) {
	outFile << it->first << ": " << it->second << "\n";
	}
	outFile.close();

	// std::string body = readFileContent("srcs/success.html");
	// HttpResponse hRes(200, body, "text/html");
	HttpResponse hRes(303, "", "text/html");
	hRes.setLocationHeader("/success.html");
	return hRes.toString();
}

const std::string	ReqHandler::deleteReqHandler(const HttpReqParsing& request) {
	HttpResponse hRes(404, "Error", "text/plain");
	return hRes.toString();
}

const std::string	ReqHandler::defaultHandler(const HttpReqParsing& request) {
	HttpResponse hRes(404, "Error", "text/plain");
	std::string response = hRes.toString();
	return response;
}

const std::string	ReqHandler::handleRequest(const HttpReqParsing& request) {
	if (request.getMethod() == "GET") {
		return getReqHandler(request);
	} else if (request.getMethod() == "POST") {
		return postReqHandler(request);
	} else if (request.getMethod() == "DELETE") {
		return deleteReqHandler(request);
	} else {
		return defaultHandler(request);
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
