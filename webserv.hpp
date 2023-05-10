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

// #include <sys/types.h>
#include <sys/stat.h>
// struct stat info;

typedef struct s_server
{
	std::string version;
	std::string port;
	std::string host;
	std::string server_name;
	std::string error_page;
	std::string client_max_body_size;
	std::string root;
	std::string cgi;
	std::vector<std::string> location;
	std::string index;

	std::string http_redirection;
	std::string http_path;
	struct s_server *next;
}	t_server;

class t_req
{
	public:
		t_req() {};
		~t_req() {};

		std::string uri;
		std::string method;
		std::string http_version;
		std::string type;
		int			autoindex;
		std::string path;
		std::string index;
};

// typedef struct s_request
// {
// 	std::string uri;
// 	std::string method;
// 	std::string http_version;
// 	std::string type;
// 	int			autoindex;
// 	std::string path;
// 	std::string index;
// }	t_request;

// typedef struct s_response
// {
// 	std::string content_length;
// 	std::string content_type;
// }	t_response;

class t_res
{
	public:
		t_res() {};
		~t_res() {};

		std::string http_version;
		int			status;
		std::string status_message;
		int			content_length;
		std::string content_type;
		std::string path;
		std::string data;
		std::string body;
};

// typedef struct s_resource
// {
// 	std::string content;
// 	std::string type;
// }	t_resource;

// Parsing
void _config_parser( t_server *_server, std::string s, int count );

// Socket
void _socket( t_server *_server, t_req *_request, t_res *_response );

// Methodes
void	_get( t_res *_response, t_req *_request, t_server *_server );
void _post();
void _delete();

// CGI
void _cgi( t_server *_server, t_res *_response );

// Request
void	_request( t_server *_server, t_req *_request, char *s );

// Response
void	_response( t_res *_response, t_req *_request );

#endif