/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-asri <ael-asri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:15:32 by ael-asri          #+#    #+#             */
/*   Updated: 2023/05/11 11:15:34 by ael-asri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

void print_error(std::string s)
{
	std::cerr << s << std::endl;
    exit(1);
}

// int has_index_files( Server &_server, Request *_request )
// {
// 	std::ifstream _file;
// 	for (size_t i=0; i < _request->index.size(); i++)
// 	{
// 		std::cerr << _server->root+_request->uri+_request->index[i] << std::endl;
// 		_file.open(_server->root+_request->uri+_request->index[i]);
// 		if (_file)
// 			return 1;
// 	}
// 	// _file.open(_server->root+_request->uri+"index.html");
// 	// if (_file)
// 	// 	return 1;
// 	return 0;
// }

void	_file_or_dir( Request *_request )
{
	struct stat info;

	if (stat(_request->root.c_str(), &info) != 0)
		print_error("path is neither a file nor a directory");

    if (S_ISDIR(info.st_mode))
		_request->type = "directory";
	else if (S_ISREG(info.st_mode))
		_request->type = "file";
	else
		print_error("path is neither a file nor a directory");
}

void	_get( Response *_response, Request *_request, Server &_server )
{
	(void)_server;
	// std::cerr << "request path: " << _request->path << std::endl;
	std::ifstream _file;
	_file.open(_request->path);
	_file ? _get_res_body(_request, _response) : _response->status = 404;

    _file_or_dir(_request);
	if (_request->type == "directory")
	{
		if (_request->path[_request->path.size()-1] != '/')
		{
			_request->path+='/';
			_response->status = 301;
		}
		else
		{
			std::cerr << "Yoo:" << _request->index.size() << " - " << _request->autoindex << std::endl;
			if (_request->index.size())
				_cgi(_request, _response);
			else
			{
				// autoindex
				if (!_request->autoindex)
					_response->status = 403;
				else
				{
					_response->status = 200;
					_response->body = "<html><body><h1>Directory file listing: </h1></body></html>";
				}
			}
		}
	}
	else if (_request->type == "file")
	{
		if (_request->cgi.size())
			_cgi(_request, _response);
		else
		{
			_response->status = 200;
			_get_res_body(_request, _response);
		}
	}
}
/*
void _post( Response *_response, Request *_request, t_server *_server )
{
	std::cout << "POST" << std::endl;
	if (_request->client_body_upload)
	{
		// Upload the shit
		// std::cerr << "body: " << body << std::endl;
	}
	else
	{
		std::ifstream _file;
		_file.open(_request->path);
		_file ? _get_res_body(_request, _response) : _response->status = 404;

		_file_or_dir(_request);
		if (_request->type == "directory")
		{
			if (_request->uri[_request->uri.size()-1] != '/')
			{
				_request->uri+='/';
				_response->status = 301;
			}
			else
			{
				if (has_index_files(_server, _request))
				{
					if (_request->cgi.size())
						_cgi(_server, _response);
					else
						_response->status = 403;
				}
				else
					_response->status = 403;
			}
		}
		else if (_request->type == "file")
		{
			if (_request->cgi.size())
				_cgi(_server, _response);
			else
			{
				_response->status = 200;
				_get_res_body(_request, _response);
			}
		}
	}
}
*/
void _delete()
{
	std::cout << "DELETE" << std::endl;
}