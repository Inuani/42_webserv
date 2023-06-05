Web Server Project
This is a web server project developed using C++.

Description
The web server is implemented using the C++ programming language and follows the HTTP/1.1 protocol specifications. It can handle HTTP GET and POST requests and supports serving static files as well as executing PHP and Python CGI scripts.

Features
HTTP/1.1 protocol support
Handling of HTTP GET and POST requests
Serving static files
Execution of PHP and Python CGI scripts
Basic error handling and logging
Requirements
To compile and run the web server, you need the following:

C++98-compatible compiler (e.g., GCC)
Make build system
Python 3.x (for Python CGI support)
Installation
Clone the repository:

bash
Copy code
git clone https://github.com/yourusername/webserv.git
Change into the project directory:

bash
Copy code
cd webserv
Build the web server using the Makefile:

bash
Copy code
make
Usage
To start the web server, run the compiled binary:

bash
Copy code
./webserv [config_file]
config_file (optional): Path to the server configuration file. If not provided, the default configuration file config/webserv.conf will be used.
The server will start listening for incoming connections on the specified port (default: 8080) as configured in the configuration file.

Configuration
The server can be configured using a configuration file in INI format. The default configuration file is config/webserv.conf, but you can specify a different configuration file when starting the server.

The configuration file includes settings such as the server port, document root, and supported file types for static file serving. The file is divided into sections, each specifying a particular aspect of the server's configuration.

An example configuration file can be found in config/webserv.conf.example. Please refer to that file for more information on the available configuration options.
