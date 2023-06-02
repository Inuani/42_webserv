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
#include "dirent.h"

ReqHandler::ReqHandler() {}

ReqHandler::ReqHandler(const Settings& settings) : _settings(settings) {}

const std::string	ReqHandler::_getReqHandler(const HttpReqParsing& request) {
	if (_reqLocation)
	{
		std::string response = _handleDirListing(_reqLocation->dir_listing, _reqLocation->index);
		if (!response.empty())
			return (response);
	}
	else
	{
		std::string response = _handleDirListing(_settings.dir_listing, _settings.index);
		if (!response.empty())
			return (response);
	}
	std::string redirectYes = _settings.redirect[_fileName];
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
	std::string contentType = request.getHeadersValue("Content-Type");
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
		std::string boundary = contentType.substr(contentType.find("boundary=") + 9, contentType.size() - 10 - contentType.find("boundary="));
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
}

const std::string	ReqHandler::_deleteReqHandler() {
	if (_fileName == "/delete_submissions") {
		if (remove("./data/form_submissions.txt") != 0) {
			throw 500;
		}
		HttpResponse hRes(303, "");
		hRes.setHeaders("Content-Type", "text/html");
		hRes.setHeaders("Location", "/success.html");
		return hRes.toString();
	}
	else if (_fileName == "/delete_all") {
		std::system("rm -rf ./data/*");
		HttpResponse hRes(303, "");
		hRes.setHeaders("Content-Type", "text/html");
		hRes.setHeaders("Location", "/success.html");
		return hRes.toString();
	}
	throw 404;
	return "";
}

void	ReqHandler::_setEnvCgi(const HttpReqParsing& request, std::string& serverPath, std::vector<std::string>& env, std::string& cookies)
{
	env.push_back("REQUEST_METHOD=" + request.getMethod());
	env.push_back("QUERY_STRING=" + request.getMethod());
	env.push_back("SCRIPT_NAME=" + serverPath + _fileName); // do not include query string
	env.push_back("REQUEST_URI=" + serverPath + _fileName + request.getQueryString()); // do include query string
	if (_reqLocation)
		env.push_back(("DOCUMENT_ROOT=" + _reqLocation->root).c_str());
	else
		env.push_back("DOCUMENT_ROOT=" + _settings.root);
	env.push_back("SCRIPT_FILENAME=" + _fullPath);
	env.push_back("SERVER_PROTOCOL=" + request.getVersion());
	env.push_back("PATH_INFO=" + request.getPathInfo());
	env.push_back("SERVER_SOFTWARE=WebservGaming/1.0");
	env.push_back("HTTP_USER_AGENT=" + request.getHeadersValue("User-Agent"));
	env.push_back("HTTP_ACCEPT=" + request.getHeadersValue("Accept"));
	env.push_back("HTTP_HOST=" + request.getHeadersValue("Host"));
	env.push_back("REDIRECT_STATUS=200");
	if (request.getMethod() == "POST") {
		env.push_back("CONTENT_TYPE=" + request.getHeadersValue("Content-Type"));
		std::ostringstream ss;
		ss << "CONTENT_LENGTH=" << request.getBody().size();
		env.push_back(ss.str());
		// env.push_back("CONTENT_LENGTH=" + std::to_string(request.getBody().size()));
	}
	if (!cookies.empty()) {
		env.push_back("HTTP_COOKIE=" + cookies);
	}
	// env.push_back(NULL);
}

void	ReqHandler::_childCgi(int fd[2], const HttpReqParsing& request, std::vector<std::string>& env, char *args[]) {
	pid_t	pid = fork();
	// for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); it++)
	// 		std::cout << *it << std::endl;

	if (pid == 0) {

		std::vector<char*> c_env;
		for(std::vector<std::string>::iterator it = env.begin(); it != env.end(); ++it) {
			c_env.push_back((char *)(it->c_str()));
		}
		c_env.push_back(NULL);

		alarm(5);
		signal(SIGALRM, exit);
		
		int bodyFd[2];
		if (pipe(bodyFd) < 0) {
			std::cerr << "Pipe Error" << std::endl;
			std::exit(2);
		}
	
	if (request.getMethod() == "POST") {
			write(bodyFd[1], request.getBody().c_str(), request.getBody().size());
			close(bodyFd[1]);
	}
	
		dup2(bodyFd[0], STDIN_FILENO);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		if (execve(args[0], args, &c_env[0]) == -1) {
			std::cerr << "execve failed" << std::endl;
			std::exit(2);
		}
	}
	else if (pid < 0) {
		throw 500;
	}

	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
		if (status == 1)
			throw 504;
		else
			throw 500;
	}
	close(fd[1]);
}

void	ReqHandler::_handleCgiReponse(int fd[2], std::multimap<std::string, std::string>& headersMap, std::string& body) {
	std::vector<char> buffer(1024);
	std::string cgiOutput;
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
}

const std::string	ReqHandler::_cgiHandler(const HttpReqParsing& request) {
	std::string serverPath;
	if (_reqLocation)
		serverPath = _reqLocation->path;
	// else
	// 	serverPath = _settings.root;
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
	std::vector<std::string> env;
	_setEnvCgi(request, serverPath, env, cookies);
	// for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); it++)
	// 	std::cout << *it << std::endl;

	// std::vector<char*> env;
	
	// env.push_back(strdup(("REQUEST_METHOD=" + request.getMethod()).c_str()));
	// env.push_back(strdup(("QUERY_STRING=" + request.getMethod()).c_str()));
	// env.push_back(strdup(("SCRIPT_NAME=" + serverPath + _fileName).c_str())); // do not include query string
	// env.push_back(strdup(("REQUEST_URI=" + serverPath + _fileName + request.getQueryString()).c_str())); // do include query string
	// if (_reqLocation)
	// 	env.push_back(strdup(("DOCUMENT_ROOT=" + _reqLocation->root).c_str()));
	// else
	// 	env.push_back(strdup(("DOCUMENT_ROOT=" + _settings.root).c_str()));
	// env.push_back(strdup(("SCRIPT_FILENAME=" + _fullPath).c_str()));
	// env.push_back(strdup(("SERVER_PROTOCOL=" + request.getVersion()).c_str()));
	// env.push_back(strdup(("PATH_INFO=" + request.getPathInfo()).c_str()));
	// env.push_back(strdup(("SERVER_SOFTWARE=WebservGaming/1.0")));
	// env.push_back(strdup(("HTTP_USER_AGENT=" + request.getHeadersValue("User-Agent")).c_str()));
	// env.push_back(strdup(("HTTP_ACCEPT=" + request.getHeadersValue("Accept")).c_str()));
	// env.push_back(strdup(("HTTP_HOST=" + request.getHeadersValue("Host")).c_str()));
	// env.push_back(strdup("REDIRECT_STATUS=200"));
	// if (request.getMethod() == "POST") {
	// 	env.push_back(strdup(("CONTENT_TYPE=" + request.getHeadersValue("Content-Type")).c_str()));
	// 	env.push_back(strdup(("CONTENT_LENGTH=" + std::to_string(request.getBody().size())).c_str()));
	// }
	// if (!cookies.empty()) {
	// 	env.push_back(strdup(("HTTP_COOKIE=" + cookies).c_str()));
	// }
	// env.push_back(NULL);

	
	_childCgi(fd, request, env, args);
	// pid_t	pid = fork();
	// if (pid == 0) {

	// 	alarm(5);
	// 	signal(SIGALRM, exit);
		
	// 	int bodyFd[2];
	// 	if (pipe(bodyFd) < 0) {
	// 		std::cerr << "Pipe Error" << std::endl;
	// 		std::exit(2);
	// 	}
	
	// if (request.getMethod() == "POST") {
	// 		write(bodyFd[1], request.getBody().c_str(), request.getBody().size());
	// 		close(bodyFd[1]);
	// }
	
	// 	dup2(bodyFd[0], STDIN_FILENO);
	// 	dup2(fd[1], STDOUT_FILENO);
	// 	close(fd[0]);
	// 	close(fd[1]);
	// 	if (execve(args[0], args, &env[0]) == -1) {
	// 		std::cerr << "execve failed" << std::endl;
	// 		std::exit(2);
	// 	}
	// }
	// else if (pid < 0) {
	// 	throw 500;
	// }

	// int status;
	// waitpid(pid, &status, 0);
	// if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
	// 	if (status == 1)
	// 		throw 504;
	// 	else
	// 		throw 500;
	// }
	// close(fd[1]);

	// std::vector<char> buffer(1024);
	// std::string cgiOutput;
	std::multimap<std::string, std::string> headersMap;
	std::string body;

	_handleCgiReponse(fd, headersMap, body);
	// try
	// {
	// 	while (true)
	// 	{
	// 		ssize_t n = read(fd[0], &buffer[0], buffer.size());
	// 		if (n <= 0) {
	// 			break;
	// 		}
	// 		cgiOutput.append(buffer.begin(), buffer.begin() + n);
	// 	}
	// 	close(fd[0]);
	// 	bool inBody = false;

	// 	std::istringstream stream(cgiOutput);
	// 	for (std::string line; std::getline(stream, line);) {
	// 		if (!inBody)
	// 		{
	// 			size_t delim = line.find(":");
	// 			if (delim == std::string::npos)
	// 			{
	// 				inBody = true;
	// 				body += line + "\n";
	// 			}
	// 			else
	// 			{
	// 				std::string key = line.substr(0, delim);
	// 				std::string value = line.substr(delim + 2);
	// 				headersMap.insert(std::make_pair(key, value));
	// 			}
	// 		}
	// 		else
	// 		{
	// 			body += line + "\n";
	// 		}
	// 	}
	// }
	// catch (const std::exception& err)
	// {
	// 	throw 500;
	// }

	// for(std::vector<char *>::iterator it = env.begin(); it != env.end(); ++it)
	// 	free(*it);
	// env.clear();
	HttpResponse hRes(200, body);
	for (std::multimap<std::string, std::string>::const_iterator it = headersMap.begin(); it != headersMap.end(); ++it) {
		hRes.setHeaders(it->first, it->second);
	}
	std::string response = hRes.toString();
	return response;
}

const std::string	ReqHandler::handleRequest(const HttpReqParsing& request) 
{
	_filePath = request.getFileDir();
	_fileName = request.getFileName();
	_reqLocation = request.getFileLocation();
	_fullPath = _filePath + _fileName;
	// std::cout << "Full path in HandleRequest : " << _filePath << _fileName << std::endl;
	if (_fileName.find(".") != std::string::npos && (request.getfileExt() == "php" || request.getfileExt() == "py")) {
		if (_reqLocation == NULL) 
		{
			// we must have a CGI ?
			// use settings
		}
		else if (_reqLocation->methods.find(request.getMethod()) == std::string::npos)
			throw 405;
		return _cgiHandler(request);
	}
	if (request.getMethod() == "GET")
	{
		if (_reqLocation != NULL && _reqLocation->methods.find(request.getMethod()) == std::string::npos)
			throw 405;
		return _getReqHandler(request);
	}
	else if (request.getMethod() == "POST")
	{
		if (_reqLocation != NULL && _reqLocation->methods.find(request.getMethod()) == std::string::npos)
			throw 405;
		return _postReqHandler(request);
	}
	else if (request.getMethod() == "DELETE")
	{
		if (_reqLocation != NULL && _reqLocation->methods.find(request.getMethod()) == std::string::npos)
			throw 405;
		return _deleteReqHandler();
	}
	throw 501;
	return "";
}

std::string ReqHandler::_handleDirListing(std::string dirListing, std::string locIndex)
{
	// std::cout << "location dirlisting is " << dirListing << std::endl;
	DIR* dirHandle = opendir(_fullPath.c_str());
	if (dirHandle != NULL)
	{
		if (dirListing == "on")
		{
			std::cout << "Full path given to replisting func : "<< _fullPath << std::endl;
			const std::string dirContent = repertoryListing(_settings, _fullPath);
			closedir(dirHandle);
			if (!dirContent.empty())
			{
				HttpResponse hRes(200, dirContent);
				std::string response = hRes.toString();
				return response;
			}
		}
		else if (locIndex.empty())
		{
			closedir(dirHandle);
			throw 403;
		}
		closedir(dirHandle);
	}
	if (_fileName == "/")
	{
		if (dirListing == "off" && locIndex.empty())
			throw 403;
		_fileName = _reqLocation->index;
	}
	return ("");
}

ReqHandler::ReqHandler(const ReqHandler &src) {
	*this = src;
}

ReqHandler::~ReqHandler() {}

ReqHandler&	ReqHandler::operator=(const ReqHandler& rhs) {
	(void)rhs;
	return *this;
}
