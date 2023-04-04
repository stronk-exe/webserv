#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <map>


typedef struct s_server
{
	std::string port;
	std::string host;
	std::string server_name;
	std::string error_page;
	std::string client_max_body_size;
	std::string location;

	std::string http_redirection;
	std::string http_path;
	struct s_server *next;
}	t_server;

typedef struct s_request
{
	std::string method;
}	t_request;

// Parsing
void _parser( t_server *_server, std::string s, int count );

// Socket
void _socket( t_server *_server );

#endif