/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-asri <ael-asri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:15:55 by ael-asri          #+#    #+#             */
/*   Updated: 2023/05/11 11:42:43 by ael-asri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	int			max_body_size;
	std::string root;
	std::string cgi;
	std::vector<std::string> location;
	std::string index;

	std::string http_redirection;
	std::string http_path;
	struct s_server *next;
}	t_server;

// class Server
// {
// 	public:
// 		Server() {};
// 		~Server() {};

// 		std::string version;
// 		std::string port;
// 		std::string host;
// 		std::string server_name;
// 		std::string error_page;
// 		std::string max_body_size;
// 		// std::string root;
// 		std::string cgi;
// 		// std::vector<std::string> location;
// 		std::string index;

// 		std::string http_redirection;
// 		std::string http_path;
// };

// class Location_block : public Server
// {
// 	public:
// 		Location_block() {};
// 		~Location_block() {};

// 		std::string	_location;
// 		std::string root;
// 		std::string allowed_methodes;
// 		int			autoindex;
// 		std::string index
// 		std::string cgi_pass;
// };

class Request
{
	public:
		Request() {};
		~Request() {};

		std::string uri;
		std::string method;
		// std::string http_version;
		std::string type;
		int			autoindex;
		std::string path;
		std::string index;
		std::string redirection;
		std::string cgi;
		int			client_body_upload;
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
// }	Requestuest;

// typedef struct s_response
// {
// 	std::string content_length;
// 	std::string content_type;
// }	Responseponse;

class Response
{
	public:
		Response() {};
		~Response() {};

		// std::string http_version;
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
// }	Responseource;

// Parsing
void _config_parser( t_server *_server, std::string s, int count );

// Socket
void _socket( t_server *_server, Request *_request, Response *_response );

// Methodes
void	_get( Response *_response, Request *_request, t_server *_server );
void _post( Response *_response, Request *_request, t_server *_server );
void _delete();

// CGI
void _cgi( t_server *_server, Response *_response );

// Request
void	_request( t_server *_server, Request *_request, Response *_response, char *s );

// Response
void	_response( Response *_response, Request *_request );
int		_get_res_body( Request *_request, Response *_response );

// Utils
void	print_error(std::string s);

#endif