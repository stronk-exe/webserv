/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:15:55 by ael-asri          #+#    #+#             */
/*   Updated: 2023/06/22 16:50:09 by mait-jao         ###   ########.fr       */
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
#include <fcntl.h>
#include <sstream>
#include <vector>
#include <map>
#include <sys/stat.h>

#define _POSIX_SOURCE
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#undef _POSIX_SOURCE
#include <stdio.h>

extern std::string webserv_loc;

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

struct CGI
{
    std::string		extension;
    std::string     path;

	CGI &operator= (const CGI & _cgi)
	{
		this->path = _cgi.path;
		this->extension = _cgi.extension;
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
    std::string					root_location, name, uploadDir;
    std::vector<std::string> 	index;
    std::vector<std::string>	allows_methods;
	Redirection					redirection;
    std::vector<CGI>			cgi_pass;

	Location& operator=(const Location& loc)
	{
		this->name = loc.name;
		this->root_location = loc.root_location;
		this->autoindex = loc.autoindex;
		this->index = loc.index;
		this->allows_methods = loc.allows_methods;
		this->redirection = loc.redirection;
		this->cgi_pass = loc.cgi_pass;
		this->uploadDir = loc.uploadDir;
		return *this;
	}
};
    

struct Server
{
    std::string					name, root_location, client_max_body_size;
    std::vector<std::string>	index;
    std::vector<Location>		locations;
    std::vector<error_page>		errors;
	Redirection					redirection;
    size_t						listen_port;

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
	std::string					file;
	std::vector<std::string>	data;
	std::vector<Server>			servers;
};

class Request
{
	public:
		Request() {};
		~Request() {};

		int									fd;
		std::string							uri;
		std::string							method;
		std::string 						type;
		int									autoindex;
		std::string							path;
		std::map<std::string, std::string>	paths;
		std::vector<std::string>			index;
		std::string							root;
		std::vector<std::string>			redirection;
		std::vector<CGI>					cgi;
		std::string							queryString;
		char								**env;
		int									client_body_upload;
		std::map<std::string, std::string>	headers;
		std::string							body;
		std::vector<error_page>				error_pages;
		int									is_method_allowed;
		std::string							upload_path;

		// Uploads
		std::string	upload_name;
		std::string	upload_content_type;
		std::string	upload_file_name;
		std::string	upload_data;
		std::string	boundary;
};

class Response
{
	public:
		Response() {};
		~Response() {};

		int			status;
		std::string status_message;
		int			content_length;
		std::string content_type;
		std::string path;
		std::string data;
		std::string body;
		std::string	location;
		std::map<std::string, std::string>	mims;
};


// Socket
void	_socket( Parsing &_server, Request *_request, Response *_response );

// Methodes
void	_get( Response *_response, Request *_request, Server &_server );
void	_post( Response *_response, Request *_request, Server &_server );
void	_delete( Response *_response, Request *_request ,Server &_server );

// CGI
void	_cgi( Request *_request, Response *_response, Server &_server );

// Request
void	_request( Parsing &_server, Server &_s, Request *_request, Response *_response, std::string s );

// Response
void		_response( Response *_response, Request *_request );
int			_get_res_body( Request *_request, Response *_response );
void    	get_indexed_file_data( Request *_request, Response *_response, std::string path );
std::string	_get_ex( std::string _file_name );

// Utils
void	print_error(std::string s);

// Parsing
void	error(std::string err);
int		str_to_num(std::string str);
void	parss_info(Parsing &parss);
void	info_autoindex(Location &loc, std::string &str);
void	split_conf(std::vector<std::string> &data, std::string str);
void	info_err_status(std::vector<error_page> &errors, std::vector<std::string>::iterator &it);
void	info_location(std::vector<Location> &locations, std::vector<std::string>::iterator &it);
void	print_data(Parsing &parss);
std::vector<std::string>	info_(std::vector<std::string>::iterator &it);

#endif