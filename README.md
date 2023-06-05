# Web Server Project

This project is a custom implementation of a web server, written entirely in C++. It's designed to handle HTTP/1.1 protocol and is capable of serving HTTP GET and POST requests. It is equipped with capabilities to serve static files as well as execute CGI scripts written in PHP and Python.

## Features

The web server supports the following features:

- Full HTTP/1.1 protocol support, including the ability to handle HTTP GET and POST requests.
- Ability to serve static files from the file system.
- Support for executing PHP and Python CGI scripts, enabling dynamic content generation.
- Basic error handling and logging to help identify and resolve issues.
- Customizable server configuration via a config file.

## Requirements

In order to compile and run this web server, you'll need the following:

- A C++98-compatible compiler, such as GCC or Clang.
- The Make build automation tool.
- Python 3.x (for Python CGI support).
- PHP (for PHP CGI support).

## Installation

1. Clone the repository to your local machine:

```bash
git clone https://github.com/yourusername/webserv.git
