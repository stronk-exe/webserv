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

std::string	_get_listed_dir( Request *_request )
{
	DIR *dir;
	struct dirent *entry;

	std::string s;

	if ((dir = opendir(_request->root.c_str())) == NULL)
		perror("opendir() error");
	else
	{
		// puts("contents of root:");
		while ((entry = readdir(dir)) != NULL)
		{
			std::string data_name = entry->d_name;
			s += "<h3><a href="+data_name+">"+data_name+"</a><br/></h3>";
			// std::cerr << "s: " << s << std::endl;
			// v.push_back(entry->d_name);
			// printf("  %s\n", entry->d_name);
		}
		closedir(dir);
	}
	// exit(1);
	return s;
	// return v;
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

void	_file_or_dir( Request *_request, Response *_response )
{
	struct stat info;

	std::cerr << "stat: " << _request->path << std::endl;
	if (stat(_request->path.c_str(), &info) != 0)
		_response->status = 404;
		// print_error("path is neither a file nor a directory");

    if (S_ISDIR(info.st_mode))
		_request->type = "directory";
	else if (S_ISREG(info.st_mode))
		_request->type = "file";
	else
		_response->status = 404;
		// print_error("path is neither a file nor a directory");
}

void	_get( Response *_response, Request *_request, Server &_server )
{
	(void)_server;
	// std::cerr << "request path: " << _request->path << std::endl;
	std::ifstream _file;
	_file.open(_request->path);
	_file ? _get_res_body(_request, _response) : _response->status = 404;

    _file_or_dir(_request, _response);
	if (_request->type == "directory")
	{
		if (_request->path[_request->path.size()-1] != '/')
		{
			_request->path+='/';
			_response->status = 301;
		}
		else
		{
			if (_request->index.size())
				_cgi(_request, _response);
			else
			{
				// autoindex
				if (!_request->autoindex)
					_response->status = 403;
				else
				{
					// std::cerr << "Yoo:" << _request->index.size() << " - " << _request->autoindex << std::endl;
					_response->status = 200;
					// _request->uri += "index.html";

					// if is html file ()
					_response->body = _get_listed_dir(_request);//"<html><body><h1>Directory file listing: </h1></body></html>";
				}
			}
		}
	}
	else if (_request->type == "file")
	{
		if (_request->cgi.size())
		{
			// std::cerr << "wzzupp" << std::endl;
			_cgi(_request, _response);
		}
		else
		{
			_response->status = 200;
			_get_res_body(_request, _response);
		}
	}
}

void _body_parser( Request *_request )
{
	std::cerr << _request->body << std::endl;
	
	std::vector<std::string> _req;
	std::string delimiter = "\r\n\r\n";
    std::string r = s;
    size_t pos = r.find(delimiter);
    std::string header = r.substr(0, pos);
    _request->body = r.substr(pos + delimiter.length());

	std::vector<std::string> v;
    std::string line;
    delimiter = "\r\n";
    // pos = 0;
    while ((pos = header.find(delimiter)) != std::string::npos)
	{
        line = header.substr(0, pos);
        v.push_back(line);
        header.erase(0, pos + delimiter.length());
    }
    v.push_back(header);

	// extract the method, the uri and the http-version
	_extract_first_line(_request, v[0]);

	std::string key, value;
	for (size_t i=0; i < v.size(); i++)
	{
		pos = v[i].find(":");
		if (pos != std::string::npos) {
			key = v[i].substr(0, pos);
			value = v[i].substr(pos + 2);
			_request->headers[key] = value;
		}
	}
}

void _post( Response *_response, Request *_request, Server &_server )
{
	std::cout << "POST" << std::endl;
	std::cerr << "zbla: " << _request->headers["Content-Type"] << std::endl;
	if (_request->headers["Content-Type"].substr(0, 19) == "multipart/form-data")
	{
		// Upload the shit
		std::cerr << "yes we do support that\nrequest body:\n" << _request->body << std::endl;
		_body_parser(_request);
	}
	else
	{
		std::ifstream _file;
		_file.open(_request->path);
		_file ? _get_res_body(_request, _response) : _response->status = 404;

		_file_or_dir(_request, _response);
		if (_request->type == "directory")
		{
			if (_request->path[_request->path.size()-1] != '/')
			{
				_request->path+='/';
				_response->status = 301;
			}
			else
			{
				if (_request->index.size())
				{
					if (_request->cgi.size())
						_cgi(_request, _response);
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
				_cgi(_request, _response);
			else
			{
				_response->status = 200;
				_get_res_body(_request, _response);
			}
		}
	}
}

void _delete()
{
	std::cout << "DELETE" << std::endl;
}
