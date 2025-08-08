# webserv

> "webserv" Project – École 42 Paris

## Overview

`webserv` is a high-performance HTTP web server implemented in C++ as part of the 42 Paris curriculum. This project covers end-to-end server development: from parsing configuration files and HTTP requests to managing connections, serving static content, and executing CGI scripts.

## Key Features

- **Configuration Parsing:** Reads and validates custom server configuration files (`miniconfig.txt`).
- **HTTP/1.1 Support:** Handles GET, POST, DELETE methods, persistent connections, chunked transfer encoding.
- **Static File Serving:** Serves HTML, CSS, JavaScript, images from designated `static_pages` directories.
- **CGI Execution:** Executes CGI scripts placed in `cgi-bin`, allowing dynamic content generation.
- **Error Handling:** Custom error pages served from `static_pages/error_pages` for standard HTTP error codes.
- **Concurrency:** Manages multiple client connections using multi-threading and non-blocking sockets.
- **Logging:** Detailed request and error logs via a flexible logging module.

## Installation & Usage

```bash
git clone https://github.com/maecarva/webserv.git
cd webserv
make

# Run the server with the provided config
./webserv miniconfig.txt
```

- Access the site at: http://localhost:PORT/site (as defined in `miniconfig.txt`)
- Place custom CGI scripts in `cgi-bin/` to enable dynamic responses.

To stop the server, press `Ctrl+C`.

## Skills Gained

- Designed a **custom config parser** to load and validate server settings.
- Implemented **HTTP/1.1 protocol** handling: header parsing, methods, status codes.
- Utilized **socket programming** (BSD sockets) and **multi-threading** for scalable request processing.
- Developed **CGI** support to bridge server and external executables.
- Managed **static and dynamic content** delivery with robust error handling.
- Built a modular **logging system** for monitoring and debugging.

## Project Structure

```plaintext
webserv/
├── cgi-bin/            	# Executable scripts for CGI handling
├── inc/                	# Header files for all modules
├── Makefile            	# Build rules for compilation and clean-up
├── miniconfig.txt      	# Sample configuration file (server blocks, routes)
├── src/                	# Source code directory
│   ├── Config/         	# Configuration parser and validation
│   ├── Logger/         	# Logging utilities (access, error logs)
│   ├── Request/        	# HTTP request parsing modules
│   │   └── Response/   	# HTTP response construction modules
│   ├── Server/         	# Core server logic, connection handling, threading
│   └── Utils/          	# Helper functions (string ops, error handlers)
└── static_pages/       	# Static content and error pages
    ├── error_pages/    	# Custom HTML pages for HTTP error codes
    └── site/           	# HTML/CSS/JS assets for the hosted website
```