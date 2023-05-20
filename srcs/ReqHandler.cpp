#include "ReqHandler.hpp"
#include <iostream>
#include "utils.hpp"
#include <sstream>
#include <map>
#include <fstream>
#include <stdio.h>
#include <unistd.h>

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
	// HttpResponse hRes(getResponseCode(filePath), body, getFileType(filePath));
	HttpResponse hRes(getResponseCode(filePath), body);
	hRes.setHeaders("Content-Type", getFileType(filePath));
	std::string response = hRes.toString();
	// std::cout << response << std::endl;
	return response;
}

const std::string	ReqHandler::_postReqHandler(const HttpReqParsing& request) {
	std::map<std::string, std::string> headers = request.getHeaders();
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
		// HttpResponse hRes(303, "", "text/html");
		// hRes.setLocationHeader("/success.html");
		HttpResponse hRes(303, "");
		hRes.setHeaders("Content-Type", "text/html");
		hRes.setHeaders("Location", "/success.html");
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

		// HttpResponse hRes(303, "", "text/html");
		// hRes.setLocationHeader("/uploadSuccess.html");
		HttpResponse hRes(303, "");
		hRes.setHeaders("Content-Type", "text/html");
		hRes.setHeaders("Location", "/uploadSuccess.html");
		return hRes.toString();
	}
	// HttpResponse hRes(404, "Error", "text/html");
	HttpResponse hRes(404, "Error");
	hRes.setHeaders("Content-Type", "text/plain");
	return hRes.toString();
}

const std::string	ReqHandler::_deleteReqHandler(const HttpReqParsing& request) {
	if (request.getUri() == "/delete_submissions") {
		if (remove("./data/form_submissions.txt") != 0) {
			// HttpResponse hRes(500, "Error deleting file", "text/plain");
			HttpResponse hRes(500, "Error deleting file");
			hRes.setHeaders("Content-Type", "text/plain");
			return hRes.toString();
		}
		// HttpResponse hRes(303, "", "text/html"); //not sure it is working
		// hRes.setLocationHeader("/success.html");
		HttpResponse hRes(303, ""); //not sure it is working
		hRes.setHeaders("Content-Type", "text/html");
		hRes.setHeaders("Location", "/success.html");
		return hRes.toString();
	}
	else if (request.getUri() == "/delete_all") {
		std::system("rm -rf ./data/*");
		// HttpResponse hRes(303, "", "text/html"); //not sure it is working either !
		// hRes.setLocationHeader("/success.html");
		HttpResponse hRes(303, ""); //not sure it is working
		hRes.setHeaders("Content-Type", "text/html");
		hRes.setHeaders("Location", "/success.html");
		return hRes.toString();
	}
	// HttpResponse hRes(404, "Error", "text/plain");
	HttpResponse hRes(404, "Error");
	hRes.setHeaders("Content-Type", "text/plain");
	return hRes.toString();
}

const std::string	ReqHandler::_defaultHandler(const HttpReqParsing& request) {
	// HttpResponse hRes(404, "Error", "text/plain");
	HttpResponse hRes(404, "Error");
	hRes.setHeaders("Content-Type", "text/plain");
	std::string response = hRes.toString();
	return response;
}

const std::string	ReqHandler::_phpCgiHandler(const HttpReqParsing& request) {

	int fd[2];
	if (pipe(fd) < 0) {
		std::cerr << "pipe Error" << std::endl;
		return "";
	}

	// 	int i = 0;
	// 	while (env[i] != NULL) {
	// 		printf("%s\n", env[i]);
	// 		++i;
	// 	}

	pid_t	pid = fork();
	if (pid == 0) {
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		char *args[] = {"./cgi-bin/php-cgi", NULL};
		char *env[] = {
			strdup(("REQUEST_METHOD=" + request.getMethod()).c_str()),
			strdup(("QUERY_STRING=")),
			strdup(("SCRIPT_NAME=" + request.getUri()).c_str()), // do not include query string
			strdup(("REQUEST_URI=" + request.getUri()).c_str()), // do include query string
			strdup(("DOCUMENT_ROOT=./www/html")), // make dynamic !
			strdup(("SCRIPT_FILENAME=./www/html" + request.getUri()).c_str()), //make dynamic !
			strdup(("SERVER_PROTOCOL=" + request.getVersion()).c_str()),
			// strdup("REMOTE_ADDR=" + request.getClientIP().c_str()), //is it needed sometimes ?
			strdup(("SERVER_SOFTWARE=WebservGaming/1.0")),
			strdup(("HTTP_USER_AGENT=" + request.getHeaders().at("User-Agent")).c_str()),
			strdup(("HTTP_ACCEPT=" + request.getHeaders().at("Accept")).c_str()),
			strdup(("HTTP_HOST=" + request.getHeaders().at("Host")).c_str()),
			strdup("REDIRECT_STATUS=200"), // make dynamic ? if yes how ?
			NULL
		};
		if (execve(args[0], args, env) == -1) {
			perror("execve failed");
			std::exit(EXIT_FAILURE);
		}
	}
	else if (pid < 0) {
		std::cerr << "fork Error" << std::endl;
		return "";
	}
	
	// close(fd[1]);
	// char buf[1024];
	// std::string body;
	// ssize_t n;
	// while ((n = read(fd[0], buf, sizeof(buf))) > 0) {
	// 	body.append(buf, n);
	// }
	// int status;
	// waitpid(pid, &status, 0);

	// // HttpResponse hRes(200, body, "text/html");
	// HttpResponse hRes(200, body);
	// hRes.setHeaders("Content-Type", "text/html");


	close(fd[1]);
	char buf[1024];
	std::string cgiOutput;
	ssize_t n;
	while ((n = read(fd[0], buf, sizeof(buf))) > 0) {
		cgiOutput.append(buf, n);
	}
	int status;
	waitpid(pid, &status, 0);

	std::map<std::string, std::string> headersMap;
	std::string body;
	bool inBody = false;

	std::istringstream stream(cgiOutput);
	for (std::string line; std::getline(stream, line);) {
		if (!inBody) {
			if (line == "\r") {
				inBody = true;
			} else {
				std::string key = line.substr(0, line.find(":"));
				std::string value = line.substr(line.find(":") + 2);
				headersMap[key] = value;
			}
		} else {
			body += line + "\n";
		}
	}

	HttpResponse hRes(200, body);
	for (std::map<std::string, std::string>::const_iterator it = headersMap.begin(); it != headersMap.end(); ++it) {
		hRes.setHeaders(it->first, it->second);
	}

	std::string response = hRes.toString();
	std::cout << "<--------------- cgi response --------------->" << std::endl;
	std::cout << response << std::endl;
	std::cout << "<--------------- end cgi response --------------->" << std::endl;
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
