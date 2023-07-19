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

void	_get_listed_dir( Client & _client )
{
	DIR *dir;

	if ((dir = opendir(_client._request.path.c_str())) == NULL)
	{
		_client._response.status = 404;
		strerror(errno);
	}
	else
	{
		struct dirent *entry;
		std::string data_name;
		_client._request.body = "<html>\n"
						"<head><title>Index of " + _client._request.path + "</title></head>\n"
						"<body>\n";

		data_name = ".";
		_client._request.body += "<h4><a href=\""+data_name+"/\">"+data_name+"</a><br/></h4>\n";
		data_name = "..";
		_client._request.body += "<h4><a href=\""+data_name+"/\">"+data_name+"</a><br/></h4>\n";

		while ((entry = readdir(dir)) != NULL)
		{
			data_name = entry->d_name;
			if (data_name == "." || data_name == "..")
				continue;
			if (entry->d_type == DT_DIR)
				_client._request.body += "<h4><a href=\""+data_name+"/\">"+data_name+"</a><br/></h4>\n";
			else
				_client._request.body += "<h4><a href=\""+data_name+"\">"+data_name+"</a><br/></h4>\n";
		}
		closedir(dir);
	}
	_client._response.body = _client._request.body;
}

void	_file_or_dir(  Client & _client )
{
	struct stat info;

	if (stat(_client._request.path.c_str(), &info) != 0)
		_client._response.status = 404;

    if (S_ISDIR(info.st_mode))
		_client._request.type = "directory";
	else if (S_ISREG(info.st_mode))
		_client._request.type = "file";
	else
		_client._response.status = 404;
}

void _body_parser(  Client & _client )
{
	std::map<std::string, std::string> m;

	std::vector<std::string> _req;
	std::string delimiter = "\r\n\r\n";
    size_t pos = _client._request.body.find(delimiter);
    std::string header = _client._request.body.substr(0, pos);
	_client._request.upload_data = "";
	for (size_t i=pos+4; i < _client._request.body.size(); i++)
		_client._request.upload_data += _client._request.body[i];

	size_t boundary_pos = _client._request.headers["Content-Type"].find("boundary=")+9;
	_client._request.boundary = _client._request.headers["Content-Type"].substr(boundary_pos);

    size_t psps= _client._request.upload_data.find(_client._request.boundary);
	std::string _real_data = "";

	for (size_t i=0; i < _client._request.upload_data.size() && i < psps-4; i++)
		_real_data += _client._request.upload_data[i];
	_client._request.upload_data = _real_data;

	std::vector<std::string> v;
    std::string line;
    delimiter = "\r\n";
    while ((pos = header.find(delimiter)) != std::string::npos)
	{
        line = header.substr(0, pos);
        v.push_back(line);
        header.erase(0, pos + delimiter.length());
    }
    v.push_back(header);

	std::string key, value;
	for (size_t i=0; i < v.size(); i++)
	{
		pos = v[i].find(":");
		if (pos != std::string::npos) {
			key = v[i].substr(0, pos);
			value = v[i].substr(pos + 2);
			m[key] = value;
		}
	}

	pos = m["Content-Disposition"].find(";");
	if (pos)
	{
		std::vector<std::string> v;
		std::istringstream iss(m["Content-Disposition"]);
		std::string token;

		while (std::getline(iss, token, ';'))
			v.push_back(token);
		if (v.size() == 3)
		{
			size_t _p = v[1].find("\"");

			_client._request.upload_name = v[1].substr(_p+1, v[1].size()-_p-2);
			_p = v[2].find("\"");
			_client._request.upload_file_name = v[2].substr(_p+1, v[2].size()-_p-2);
		}
	}
	_client._request.upload_content_type = m["Content-Type"];
}

void	_get( Client & _client, Server &_server )
{
    _file_or_dir(_client);

	if (!_client._response.status)
	{
		if (_client._request.type == "directory")
		{
			if (_client._request.path[_client._request.path.size()-1] != '/')
			{
				_client._request.path+='/';
				_client._response.status = 301;
				_client._response.location = _client._request.uri+'/';
			}
			else
			{
				if (_client._request.index.size())
				{
					_client._response.status = 200;
					if (_client._request.cgi.size())
						_cgi(_client, _server);
					if (_client._response.body.empty())
						get_indexed_file_data(_client);
				}
				else
				{
					// autoindex
					if (!_client._request.autoindex)
						_client._response.status = 403;
					else
					{
						_client._response.status = 200;
						_get_listed_dir(_client);
						_client._response.body = _client._request.body;
						_client._response.content_type = "text/html";
					}
				}
			}
		}
		else if (_client._request.type == "file")
		{
			_client._response.status = 200;
			if (_client._request.cgi.size())
				_cgi(_client, _server);
			else
				_get_res_body(_client, _client._request.path);
		}
	}
}

void _post(  Client & _client , Server &_server )
{
	if (!_client._response.status)
	{
		if (_client._request.upload_path.size())
		{
			_client._response.content_length = _client._request.body.size();
			_client._response.content_type = "text/html";

			_cgi(_client, _server);
			if (!_client._response.body.size())
			{
				_body_parser(_client);
				std::ofstream _upload_file(_client._request.path+'/'+_client._request.upload_file_name);
				
				_upload_file << _client._request.upload_data;
				_client._response.content_type = _client._response.mims[_get_ex(_client._request.upload_file_name)];
				_client._response.body = _client._request.body;
				_client._response.content_length = _client._response.body.size();
			}
			_client._response.status = 200;
		}
		else
		{
			_file_or_dir(_client);
			if (_client._request.type == "directory")
			{
				if (_client._request.path[_client._request.path.size()-1] != '/')
				{
					_client._request.path+='/';
					_client._response.status = 301;
				}
				else
				{
					if (_client._request.index.size())
					{
						if (_client._request.cgi.size())
							_cgi(_client, _server);
						else
							_client._response.status = 403;
					}
					else
						_client._response.status = 403;
				}
			}
			else if (_client._request.type == "file")
			{
				if (_client._request.cgi.size())
					_cgi(_client, _server);
				else
				{
					_client._response.status = 200;
					_get_res_body(_client, _client._request.path);
				}
			}
		}
	}
}

void _delete( Client & _client )
{
    _file_or_dir(_client);

	if (_client._response.status == 404)
		return ;
	if (_client._request.type == "directory")
	{
		if (_client._request.path[_client._request.path.size()-1] != '/')
			_client._response.status = 409;	//Conflict
		else
		{
			if (std::system(("rm -r " + _client._request.path).c_str())!= 0)
			{
				if (!access(_client._request.path.c_str(), W_OK))
					_client._response.status = 500;	// Internal Server Error
				else
					_client._response.status = 403;	// Forbidden
				strerror(errno);
			}
			else
				_client._response.status = 204;	// No Content
		}
	}
	else if (_client._request.type == "file")
	{

		_client._response.status = 204;	// No Content
		if (access(_client._request.path.c_str(), W_OK))
			_client._response.status = 403;	// Forbidden
		else if (std::remove(_client._request.path.c_str()) != 0)
		{
			if (!access(_client._request.path.c_str(), W_OK))
				_client._response.status = 500;	// Internal Server Error
			strerror(errno);
		}
	}
}
