#include "ReqHandler.hpp"
#include <iostream>
#include "utils.hpp"
#include <sstream>
#include <map>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

ReqHandler::ReqHandler() {}

const std::string	ReqHandler::_getReqHandler(const HttpReqParsing& request) {
	//std::string filePath = request.getUri();
	std::string filePath = "www/html";
	std::string defaultFile = "/index.html";
	if (request.getQueryString().find("file") != std::string::npos) {
		filePath.append("/" + request.getQueryValue("file"));
	} else if (request.getUri() == "/")
		filePath.append(defaultFile);
	else
		filePath.append(request.getUri());
	std::string body = readFileContent(filePath);
	HttpResponse hRes(getResponseCode(filePath), body);
	hRes.setHeaders("Content-Type", getFileType(filePath));
	std::string response = hRes.toString();
	// std::cout << response << std::endl;
	return response;
}

const std::string	ReqHandler::_postReqHandler(const HttpReqParsing& request) {
	// std::map<std::string, std::string> headers = request.getHeaders();
	// std::string contentType = headers["Content-Type"];
	std::string contentType = request.getHeadersValue("Content-Type");

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

		HttpResponse hRes(303, "");
		hRes.setHeaders("Content-Type", "text/html");
		hRes.setHeaders("Location", "/uploadSuccess.html");
		return hRes.toString();
	}
	HttpResponse hRes(404, "Error");
	hRes.setHeaders("Content-Type", "text/plain");
	return hRes.toString();
}

const std::string	ReqHandler::_deleteReqHandler(const HttpReqParsing& request) {
	if (request.getUri() == "/delete_submissions") {
		if (remove("./data/form_submissions.txt") != 0) {
			HttpResponse hRes(500, "Error deleting file");
			hRes.setHeaders("Content-Type", "text/plain");
			return hRes.toString();
		}
		HttpResponse hRes(303, ""); //not sure it is working
		hRes.setHeaders("Content-Type", "text/html");
		hRes.setHeaders("Location", "/success.html");
		return hRes.toString();
	}
	else if (request.getUri() == "/delete_all") {
		std::system("rm -rf ./data/*");
		HttpResponse hRes(303, ""); //not sure it is working
		hRes.setHeaders("Content-Type", "text/html");
		hRes.setHeaders("Location", "/success.html");
		return hRes.toString();
	}
	HttpResponse hRes(404, "Error");
	hRes.setHeaders("Content-Type", "text/plain");
	return hRes.toString();
}

const std::string	ReqHandler::_defaultHandler(const HttpReqParsing& request) {
	HttpResponse hRes(404, "Error");
	hRes.setHeaders("Content-Type", "text/plain");
	std::string response = hRes.toString();
	return response;
}

// const std::string	_pythonCgiHandler(const HttpReqParsing& request) {
// 	int fd[2];
// 	if (pipe(fd) < 0) {
// 		perror("Pipe error");
// 		return "";
// 	}


// }

const std::string	ReqHandler::_cgiHandler(const HttpReqParsing& request, const std::string& filePath) {

	int fd[2];
	if (pipe(fd) < 0) {
		perror("Pipe error");
		return "";
	}
	char *args[] = {NULL, NULL, NULL};
	if (filePath.substr(filePath.find_last_of(".") + 1) == "py") {
		args[0] = (char *)"/usr/bin/python3";
		std::string pythonPath = "./www/html" + request.getUri();
		args[1] = (char *)pythonPath.c_str();
	}
	else if (filePath.substr(filePath.find_last_of(".") + 1) == "php") {
		args[0] = (char *)"./cgi-bin/php-cgi";
	}

	std::string cookies = request.getHeadersValue("Cookie");
	std::vector<char*> env;
	env.push_back(strdup(("REQUEST_METHOD=" + request.getMethod()).c_str()));
	env.push_back(strdup(("QUERY_STRING=")));
	env.push_back(strdup(("SCRIPT_NAME=" + request.getUri()).c_str())); // do not include query string
	env.push_back(strdup(("REQUEST_URI=" + request.getUri() + request.getQueryString()).c_str())); // do include query string
	env.push_back(strdup(("DOCUMENT_ROOT=./www/html"))); // make dynamic !
	env.push_back(strdup(("SCRIPT_FILENAME=./www/html" + request.getUri()).c_str())); //make dynamic !
	env.push_back(strdup(("SERVER_PROTOCOL=" + request.getVersion()).c_str()));
		// strdup("REMOTE_ADDR=" + request.getClientIP().c_str()), //is it needed sometimes ?
	env.push_back(strdup(("SERVER_SOFTWARE=WebservGaming/1.0")));
	env.push_back(strdup(("HTTP_USER_AGENT=" + request.getHeadersValue("User-Agent")).c_str()));
	env.push_back(strdup(("HTTP_ACCEPT=" + request.getHeadersValue("Accept")).c_str()));
	env.push_back(strdup(("HTTP_HOST=" + request.getHeadersValue("Host")).c_str()));
	env.push_back(strdup("REDIRECT_STATUS=200")); // make dynamic ? if yes how ?
	if (request.getMethod() == "POST") {
		env.push_back(strdup(("CONTENT_TYPE=" + request.getHeadersValue("Content-Type")).c_str()));
		// std::cout << "CONTENT_TYPE: " << request.getHeadersValue("Content-Type") << std::endl;
		env.push_back(strdup(("CONTENT_LENGTH=" + std::to_string(request.getBody().size())).c_str()));
		// std::cout << "CONTENT_LENGTH: " << std::to_string(request.getBody().size()) << std::endl;
		// std::cout << "Request Body: " << request.getBody() << std::endl;
	}
	if (!cookies.empty()) {
		env.push_back(strdup(("HTTP_COOKIE=" + cookies).c_str()));
	}
	env.push_back(NULL);

	pid_t	pid = fork();
	if (pid == 0) {

	int bodyFd[2];
	if (pipe(bodyFd) < 0) {
		std::cerr << "Pipe Error" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	
	if (request.getMethod() == "POST") {
		std::string contentType = request.getHeadersValue("Content-Type");
		if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
			// std::cout << request.getBody() << std::endl;
			write(bodyFd[1], request.getBody().c_str(), request.getBody().size());
			close(bodyFd[1]);
		}
		else if (contentType.find("multipart/form-data") != std::string::npos) {

		}
	}
	
		dup2(bodyFd[0], STDIN_FILENO);
		// std::cout << "CONTENT_TYPE: " << request.getHeadersValue("Content-Type") << std::endl;
		// std::cout << "CONTENT_LENGTH: " << std::to_string(request.getBody().size()) << std::endl;
		dup2(fd[1], STDOUT_FILENO);
		// close(bodyFd[0]);
		close(fd[0]);
		close(fd[1]);
		if (execve(args[0], args, &env[0]) == -1) {
			std::cerr << "execve failed" << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}
	else if (pid < 0) {
		std::cerr << "fork Error" << std::endl;
		return "";
	}
	int status;
	waitpid(pid, &status, 0);

	close(fd[1]);
	std::vector<char> buffer(1024);
	std::string cgiOutput;

	while (true) {
		ssize_t n = read(fd[0], &buffer[0], buffer.size());
		if (n <= 0) {
			break;
		}
		cgiOutput.append(buffer.begin(), buffer.begin() + n);
	}
	close(fd[0]);
	std::multimap<std::string, std::string> headersMap;
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
				headersMap.insert(std::make_pair(key, value));
			}
		} else {
			body += line + "\n";
		}
	}

	HttpResponse hRes(200, body);
	for (std::multimap<std::string, std::string>::const_iterator it = headersMap.begin(); it != headersMap.end(); ++it) {
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
		return _cgiHandler(request, filePath);
	}
	else if (filePath.find_last_of(".") != std::string::npos && filePath.substr(filePath.find_last_of(".") + 1) == "py") {
		return _cgiHandler(request, filePath);
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
