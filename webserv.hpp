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
#include <vector>
#include <map>

typedef struct s_server
{
	std::string port;
	std::string host;
	std::string server_name;
	std::string error_page;
	std::string client_max_body_size;
	// std::string location;
	std::vector<std::string> location;
	std::vector<std::string> index;

	std::string http_redirection;
	std::string http_path;
	struct s_server *next;
}	t_server;

typedef struct s_request
{
	std::string uri;
	std::string method;
	std::string http_version;
	std::string type;
	int			autoindex;
}	t_request;

typedef struct s_resource
{
	std::string content;
	std::string type;
}	t_resource;

// Parsing
void _parser( t_server *_server, std::string s, int count );

// Socket
void _socket( t_server *_server );

// Methodes
void _get( t_request *_request, t_server *_server );
void _post();
void _delete();

// CGI
void _cgi();

#endif