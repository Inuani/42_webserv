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

ReqHandler::ReqHandler(const Settings& settings) : _settings(settings) {}

const std::string	ReqHandler::_getReqHandler(const HttpReqParsing& request) {
	if (_reqLocation)
	{
		if (_fileName == "/")
		{
			if (_reqLocation->dir_listing == "on")
			{
				const std::string dirContent = repertoryListing(_filePath, _reqLocation);
				if (!dirContent.empty())
				{
					HttpResponse hRes(200, dirContent);
					std::string response = hRes.toString();
					return response;
				}
			}
			_fileName = _reqLocation->index;
		}
	}
	else
	{
		if (_fileName == "/")
		{
			if (_settings.dir_listing == "on")
			{
				const std::string dirContent = repertoryListing(_filePath, _reqLocation);
				if (!dirContent.empty())
				{
					HttpResponse hRes(200, dirContent);
					std::string response = hRes.toString();
					return response;
				}
			}
			_fileName = _settings.index;
		}
	}
	std::string redirectYes = _settings.redirect[request.getUri()];
	if (!redirectYes.empty())
	{
		HttpResponse hRes(302, "");
		hRes.setHeaders("Location", redirectYes);
		return hRes.toString();
	}
	else if (request.getQueryString().find("file") != std::string::npos)
		_fileName = "/" + request.getQueryValue("file");
	// else
	// 	filePath.append(request.getUri());
	std::string body = readFileContent(_filePath + _fileName);
	HttpResponse hRes(200, body);
	hRes.setHeaders("Content-Type", getFileType(_filePath + _fileName));
	std::string response = hRes.toString();
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

		// std::string boundary = request.getBody().substr(0, 40); //caution
		std::string boundary = contentType.substr(contentType.find("boundary=") + 9, contentType.size() - 10 - contentType.find("boundary="));
		// std::cout << boundary << std::endl;
		// std::cout << request.getBody() << std::endl;
		std::string content = request.getBody().substr(request.getBody().find("\r\n\r\n") + 4, std::string::npos);
		content.erase(content.find(boundary) - 4, std::string::npos);

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
	throw 404;
	return "";
	// HttpResponse hRes(404, "Error");
	// hRes.setHeaders("Content-Type", "text/plain");
	// return hRes.toString();
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
	throw 404;
	return "";
	// HttpResponse hRes(404, "Error");
	// hRes.setHeaders("Content-Type", "text/plain");
	// return hRes.toString();
}

const std::string	ReqHandler::_defaultHandler(const HttpReqParsing& request) {
	HttpResponse hRes(404, "Error");
	hRes.setHeaders("Content-Type", "text/plain");
	std::string response = hRes.toString();
	return response;
}

const std::string	ReqHandler::_cgiHandler(const HttpReqParsing& request) {
	std::string fullPath = _filePath + _fileName;
	std::string serverPath;
	if (_reqLocation)
		serverPath = _reqLocation->path;
	int fd[2];
	if (pipe(fd) < 0) {
		throw 500;
	}
	char *args[] = {NULL, NULL, NULL};
	if (request.getfileExt() == "py") {
		args[0] = (char *)"/usr/bin/python3";
		std::string pythonPath = _filePath + _fileName;
		args[1] = (char *)pythonPath.c_str();
	}
	else if (request.getfileExt() == "php") {
		args[0] = (char *)"./cgi-bin/php-cgi";
	}

	std::string cookies = request.getHeadersValue("Cookie");
	std::vector<char *> env;
	env.push_back(strdup(("REQUEST_METHOD=" + request.getMethod()).c_str()));
	env.push_back(strdup(("QUERY_STRING=" + request.getMethod()).c_str()));
	env.push_back(strdup(("SCRIPT_NAME=" + serverPath + _fileName).c_str())); // do not include query string
	env.push_back(strdup(("REQUEST_URI=" + serverPath + _fileName + request.getQueryString()).c_str())); // do include query string
	if (_reqLocation)
		env.push_back(strdup(("DOCUMENT_ROOT=" + _reqLocation->root).c_str()));
	else
		env.push_back(strdup(("DOCUMENT_ROOT=" + _settings.root).c_str()));
	env.push_back(strdup(("SCRIPT_FILENAME=" + fullPath).c_str()));
	env.push_back(strdup(("SERVER_PROTOCOL=" + request.getVersion()).c_str()));
	env.push_back(strdup(("PATH_INFO=" + request.getPathInfo()).c_str()));
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

		alarm(5);
		signal(SIGALRM, exit);
		
		int bodyFd[2];
		if (pipe(bodyFd) < 0) {
			std::cerr << "Pipe Error" << std::endl;
			std::exit(EXIT_FAILURE);
		}
	
	if (request.getMethod() == "POST") {
		// std::string contentType = request.getHeadersValue("Content-Type");
		// if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
			// std::cout << request.getBody() << std::endl;
			write(bodyFd[1], request.getBody().c_str(), request.getBody().size());
			close(bodyFd[1]);
		// }
		// else if (contentType.find("multipart/form-data") != std::string::npos) {

		// }
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
		throw 500;
	}

	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
		throw 504;
	}

	close(fd[1]);
	std::vector<char> buffer(1024);
	std::string cgiOutput;
	std::multimap<std::string, std::string> headersMap;
	std::string body;

	try
	{
		while (true)
		{
			ssize_t n = read(fd[0], &buffer[0], buffer.size());
			if (n <= 0) {
				break;
			}
			cgiOutput.append(buffer.begin(), buffer.begin() + n);
		}
		close(fd[0]);
		bool inBody = false;

		// std::cout << cgiOutput << std::endl;
		std::istringstream stream(cgiOutput);
		for (std::string line; std::getline(stream, line);) {
			if (!inBody)
			{
				size_t delim = line.find(":");
				if (delim == std::string::npos)
				{
					inBody = true;
					body += line + "\n";
				}
				else
				{
					std::string key = line.substr(0, delim);
					std::string value = line.substr(delim + 2);
					headersMap.insert(std::make_pair(key, value));
				}
			}
			else
			{
				body += line + "\n";
			}
		}
	}
	catch (const std::exception& err)
	{
		throw 500;
	}
	// std::cout << "coucou2" << std::endl;
	
	HttpResponse hRes(200, body);
	for (std::multimap<std::string, std::string>::const_iterator it = headersMap.begin(); it != headersMap.end(); ++it) {
		hRes.setHeaders(it->first, it->second);
	}

	std::string response = hRes.toString();
	// std::cout << "<--------------- cgi response --------------->" << std::endl;
	// std::cout << response << std::endl;
	// std::cout << "<--------------- end cgi response --------------->" << std::endl;
	return response;
}

const std::string	ReqHandler::handleRequest(const HttpReqParsing& request) 
{
	_filePath = request.getFileDir();
	_fileName = request.getFileName();
	_reqLocation = request.getFileLocation();
	//std::string filePath = request.getUri();
	std::cout << "Full path in HandleRequest : " << _filePath << _fileName << std::endl;
	if (_fileName.find(".") != std::string::npos && (request.getfileExt() == "php" || request.getfileExt() == "py")) {
		if (_reqLocation == NULL) 
		{
			// use settings
		}
		else if (_reqLocation->methods.find(request.getMethod()) == std::string::npos) {
			std::cout << "405 Method Not Allowed" << std::endl;
			throw 405;
		}
		return _cgiHandler(request);
	}
	if (request.getMethod() == "GET") {
		return _getReqHandler(request);
	} else if (request.getMethod() == "POST") {
		return _postReqHandler(request);
	} else if (request.getMethod() == "DELETE") {
		return _deleteReqHandler(request);
	}
	throw 501; // Not Implemented
	return "";
	// return _defaultHandler(request);
}


ReqHandler::ReqHandler(const ReqHandler &src) {
	*this = src;
}

ReqHandler::~ReqHandler() {}

ReqHandler&	ReqHandler::operator=(const ReqHandler& rhs) {
	(void)rhs;
	return *this;
}


		// if (location != NULL) {
		// 	std::cout << "Location struct:" << std::endl;
		// 	std::cout << "Path: " << location->path << std::endl;
		// 	std::cout << "Root: " << location->root << std::endl;
		// 	std::cout << "Index: " << location->index << std::endl;
		// 	std::cout << "Methods: " << location->methods << std::endl;
		// 	std::cout << "Error Pages: " << location->err_pages << std::endl;
		// }
		// std::cout << "------" << std::endl;
		// std::cout << location->methods.find(request.getMethod()) << std::endl;
		// std::cout << "------" << std::endl;