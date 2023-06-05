# Web Server Project

This project is a custom implementation of a web server, written entirely in C++. It's designed to handle HTTP/1.1 protocol and is capable of serving HTTP GET, POST and DELETE requests. It is equipped with capabilities to serve static files as well as execute CGI scripts written in PHP and Python (only GET and POST for CGI scripts).

## Features

The web server supports the following features:

- Full HTTP/1.1 protocol support, including the ability to handle HTTP GET, POST and DELETE requests.
- Ability to serve static files from the file system.
- Support for executing PHP and Python CGI scripts, enabling dynamic content generation.
- Basic error handling and logging to help identify and resolve issues.
- Customizable server configuration via a config file.

## Requirements

In order to compile and run this web server, you'll need the following:

- A C++98-compatible compiler, such as GCC or Clang.
- The Make build automation tool.
- Python 3.x for Python CGI support (the executable for PHP CGI is included in the repo).

## Installation

1. Clone the repository to your local machine:

```bash
git clone https://github.com/yourusername/webserv.git
```

2. Navigate into the project directory:

```bash
cd webserv
```

3. Build the web server using the Makefile:

```bash
make
```

## Usage

To start the web server, run the compiled binary:

```
./webserv [config_file]
```

The config_file is optional and represents the path to the server configuration file. If not provided, the default configuration file webserv.conf will be used.

Upon launching, the server will start listening for incoming connections on the specified port (default: 8080) as configured in the configuration file.

## Configuration

The default configuration file is located at ./webserv.conf, but you can specify a different configuration file when starting the server.

The configuration file consists of various settings, including the server port, document root, supported file types for static file serving, etc. The file is split into sections, each specifying a different aspect of the server's configuration.

For reference, an example configuration file can be found in webserv.conf This example file provides an explanation for each available configuration option.
