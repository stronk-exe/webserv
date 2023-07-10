/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:15:55 by ael-asri          #+#    #+#             */
/*   Updated: 2023/06/11 20:29:24 by mait-jao         ###   ########.fr       */
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
#include <map>
#include <fcntl.h>
#include <limits.h>
#include <sstream>
#include <vector>
#include <iterator>
#include <sys/stat.h>
#include <map>


#include <errno.h>


#define _POSIX_SOURCE
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#undef _POSIX_SOURCE
#include <stdio.h>

extern std::string _webserv_loc;

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
	
	void clear(void)
	{
		return_status = 0;
		path.clear();
	}
};

struct Location
{
    bool						autoindex;
    std::string					root_location, name, uploadDir, chyata;
    std::vector<std::string> 	index;
    std::vector<std::string>	allows_methods;
	Redirection					redirection;
    std::vector<CGI>			cgi_pass;

	Location& operator=(const Location& loc)
	{
		this->name = loc.name;
		this->chyata = loc.chyata;
		this->root_location = loc.root_location;
		this->autoindex = loc.autoindex;
		this->index = loc.index;
		this->allows_methods = loc.allows_methods;
		this->redirection = loc.redirection;
		this->cgi_pass = loc.cgi_pass;
		this->uploadDir = loc.uploadDir;
		return *this;
	}

	void clear(void)
	{
		name.clear();
		chyata.clear();
		root_location.clear();
		autoindex = false;
		index.clear();
		allows_methods.clear();
		redirection.clear();
		cgi_pass.clear();
		uploadDir.clear();
	};
};
    

struct Server
{
    std::string					name, root_location, client_max_body_size, chyata;
    std::vector<std::string>	index;
    std::vector<Location>		locations;
    std::vector<error_page>		errors;
	// Redirection					redirection;
    size_t						listen_port;

	Server () { listen_port = 0; }
	Server& operator=(const Server& serv) {
		this->name = serv.name;
		this->chyata = serv.chyata;
		this->root_location = serv.root_location;
		this->client_max_body_size = serv.client_max_body_size;
		this->listen_port = serv.listen_port;
		this->index = serv.index;
		this->locations = serv.locations;
		// this->redirection = serv.redirection;
		this->errors = serv.errors;
    	return *this;
  	}

	void clear (void)
	{
		name.clear();
		root_location.clear();
		client_max_body_size.clear();
		listen_port = 0;
		index.clear();
		locations.clear();
		chyata.clear();
		// redirection.clear();
		errors.clear();
	};
};

struct Parsing
{
	std::string					file;
	std::vector<std::string>	data;
	std::vector<Server>			servers;
	
	Parsing& operator=(const Parsing& pars) {
		this->file = pars.file;
		this->data = pars.data;
		this->servers = pars.servers;
    	return *this;
  	}

};

class Request
{
	public :

		int									fd;
		std::string							uri;
		std::string							method, queryString;
		std::string 						type;
		int									autoindex;
		std::string							path;
		std::map<std::string, std::string>	paths;
		std::vector<std::string>			index;
		std::string							root;
		std::vector<std::string>			redirection;
		std::vector<CGI>					cgi;
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

		Request() {
			fd = 0;
			autoindex = 0;
			client_body_upload = 0;
			is_method_allowed = 0;
			
			env = new char*[11];
			for (int i = 0; i < 10; i++)
				env[i] = NULL;
		};
		Request(const Request & req) { *this = req;};

		Request& operator=(  const  Request & req) {
			fd = req.fd;
			uri = req.uri;
			method = req.method;
			type = req.type;
			autoindex = req.autoindex;
			path = req.path;
			paths = req.paths;
			queryString = req.queryString;
			index = req.index;
			root = req.root;
			redirection = req.redirection;
			cgi = req.cgi;
			client_body_upload = req.client_body_upload;
			headers = req.headers;
			body = req.body;
			error_pages = req.error_pages;
			is_method_allowed = req.is_method_allowed;
			upload_path = req.upload_path;

		// Uploads
			upload_name = req.upload_name;
			upload_content_type = req.upload_content_type;
			upload_file_name = req.upload_file_name;
			upload_data = req.upload_data;
			boundary = req.boundary;

			for (int i = 0; i < 10; i++)
			{
				if (env[i])
					delete env[i];
				if (req.env[i])
					env[i] = strdup(req.env[i]);
				else
					env[i] = NULL;
			}

			return *this;
		};

		~Request() {
			for (int i = 0; i < 10; i++)
			{
				if (env[i])
					delete env[i];
			}
			delete[] env;
		};

};

class Response
{
	public:

		int			status;
		std::string status_message;
		size_t		content_length;
		std::string content_type;
		std::string path;
		std::string data;
		std::string body;
		std::string	location;
		std::map<std::string, std::string>	mims;

		Response() {
			status = 0;
			content_length = 0;
		};
		Response(const Response & res) { *this = res; };

		Response& operator=(  const Response & res) { 
			status = res.status;
			status_message = res.status_message;
			content_length = res.content_length;
			content_type = res.content_type;
			path = res.path;
			data = res.data;
			body = res.body;
			location = res.location;
			mims = res.mims;
			return *this; 
		};

		~Response() {};

};


class Client {

	public:

		int		_id;
		pid_t	_cgi_pid;
		bool	_kill_pid;
		size_t	_wr;
		int		pipe_fd[2];
		std::string		file,  body, cookies;
		int		_read_status, status;
		int		_write_status, _done_writing;
		long return_write;
		int		_done_reading;
		int		_file_done_reading;
		int		fd_file, data;
		int		firstTime_HuH;
		ssize_t	post_legnth, read;
		std::string	buffer, prsing_req;
		std::string substring, s;

		Request		_request;
		Response	_response;

		Client( ) {};

		Client(const int id ) {
			_id = id;
			_wr = 0;
			data = 0;
			_read_status = 1;
			post_legnth  = 0;
			read  = 0;
			status = 0;
			_write_status = 0;
			_done_reading = 0;
			_file_done_reading = 0;
			return_write = 0;
			firstTime_HuH= 0;
			_done_writing = 0;
			fd_file = 0;
			_cgi_pid = -2;
			_kill_pid = true;
		};

		Client(const Client & client) { *this = client; };

		Client& operator= (const Client & client) { 
			_id = client._id;
			_wr = client._wr;
			data = client.data;
			read = client.read;
			post_legnth = client.post_legnth;
			prsing_req = client.prsing_req;
			return_write = client.return_write;
			status = client.status;
			_read_status = client._read_status;
			_write_status = client._write_status;
			_done_reading = client._done_reading;
			_done_writing = client._done_writing;
			fd_file = client.fd_file;
			firstTime_HuH = client.firstTime_HuH;
			_file_done_reading = client._file_done_reading;
			_cgi_pid = client._cgi_pid;
			_kill_pid = client._kill_pid;
			_request = client._request;
			body  = client.body;
			cookies  = client.cookies;
			_response = client._response;
			return *this; 
		};
		~Client() {};

		bool operator ==(Client &b ) { return _id == b._id; }



};

struct Socket 
{
	int					_socket_fd, fd_size, x, coming_socket;
    struct sockaddr_in	address;
    int					addrlen;
    int					default_port;
	std::vector<int>	_socket_fds;
	std::vector<Client> Clients;
	fd_set				_sockets, _current_sockets, _readfds, _writefds;
	Parsing 			_server;
};

// Socket
void	_socket( Parsing &_server );

//cgi
std::string generateRandomString(int length);
void parent_process(std::string &result, int *pipe_fd);
void get_body(Client & client);

// Methodes
void	_get( Client & _client, Server &_server );
void	_post( Client & _client, Server &_server );
void	_delete( Client & _client );

// CGI
std::string generateRandomString(int length, std::string ss);
void	_cgi( Client & _client , Server &_server );
std::string num_to_str(ssize_t num);

// Request
void	_request( Parsing &_server, Server &_s, Request &_request, Response &_response, std::string s );

// Response
void		_response( Client & _client );
int			_get_res_body( Client & _client );
void    	get_indexed_file_data( Request &_request, Response &_response, std::string path );
std::string	_get_ex( std::string _file_name );

// Utils
void	print_error(std::string s);

// Parsing
void	error(std::string err);
size_t str_to_num(std::string str);
void	parss_info(Parsing &parss);
void	info_autoindex(Location &loc, std::string &str);
void 	split_conf(std::vector<std::string> &data, std::string str);
void	info_err_status(std::vector<error_page> &errors, std::vector<std::string>::iterator &it);
void	info_location(std::vector<Location> &locations, std::vector<std::string>::iterator &it);
void	print_data(Parsing &parss);
std::vector<std::string>	info_(std::vector<std::string>::iterator &it);
int createFile(const char* filename, std::string data);


#endif