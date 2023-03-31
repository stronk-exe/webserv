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

#define PORT 8080

typedef struct s_server
{
	std::string port;
	std::string host;
	std::string location;
	struct s_server *next;
}	t_server;

// Parsing
void _parser( std::string txt );

#endif