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

// #include "Parsing/Parsing.hpp"


struct error_page
{
    std::vector<int>  error_status;
    std::string       path;

	error_page &operator= (const error_page & err)
	{
		this->path = err.path;
		this->error_status = err.error_status;
		return *this;
	}
};

struct Redirection
{
    size_t            return_status;
    std::string       path;

	Redirection& operator=(const Redirection& red)
	{
		this->return_status = red.return_status;
		this->path = red.path;
		return *this;
	}
};

struct Location
{
    bool						autoindex;
    std::string					root_location, name;
    std::vector<std::string> 	index;
    std::vector<std::string>	allows_methods;
	Redirection					redirection;
    std::vector<std::string>	cgi_pass;

	Location& operator=(const Location& loc)
	{
		this->name = loc.name;
		this->root_location = loc.root_location;
		this->autoindex = loc.autoindex;
		this->index = loc.index;
		this->allows_methods = loc.allows_methods;
		this->redirection = loc.redirection;
		this->cgi_pass = loc.cgi_pass;
		return *this;
	}
};
    

struct Server
{
    std::string  name, root_location, client_max_body_size;
    std::vector<std::string>  index;
    std::vector<Location>   locations;
    std::vector<error_page>  errors;
	Redirection      redirection;
    size_t  listen_port;

	Server& operator=(const Server& serv) {
		this->name = serv.name;
		this->root_location = serv.root_location;
		this->client_max_body_size = serv.client_max_body_size;
		this->listen_port = serv.listen_port;
		this->index = serv.index;
		this->locations = serv.locations;
		this->redirection = serv.redirection;
		this->errors = serv.errors;
    return *this;
  }
};

struct Parsing
{
	std::string     file;
	std::vector<std::string> data;
	std::vector<Server>  servers;
};



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
		std::string 						type;
		int									autoindex;
		std::string							path;
		std::vector<std::string>			index;
		std::string							root;
		std::vector<std::string>			redirection;
		std::vector<std::string>			cgi;
		int									client_body_upload;
		std::map<std::string, std::string>	headers;
		std::string							body;
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
void	_config_parser( t_server *_server, std::string s, int count );

// Socket
void	_socket( Parsing &_server, Request *_request, Response *_response );

// Methodes
void	_get( Response *_response, Request *_request, Server &_server );
void	_post( Response *_response, Request *_request, t_server *_server );
void	_delete();

// CGI
void _cgi( Request *_request, Response *_response );

// Request
void	_request( Parsing &_server, Server &_s, Request *_request, Response *_response, char *s );

// Response
void	_response( Response *_response, Request *_request );
int		_get_res_body( Request *_request, Response *_response );

// Utils
void	print_error(std::string s);






void error(std::string err);
int str_to_num(std::string str);
void parss_info(Parsing &parss);
void info_autoindex(Location &loc, std::string &str);
void info_(std::vector<std::string>  &vec, std::vector<std::string>::iterator &it);
void split_conf(std::vector<std::string> &data, std::string str);
void info_err_status(std::vector<error_page> &errors, std::vector<std::string>::iterator &it);
void info_location(std::vector<Location> &locations, std::vector<std::string>::iterator &it);
void print_data(Parsing &parss);

#endif