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

// Parsing
void _parser( std::string txt );

#endif