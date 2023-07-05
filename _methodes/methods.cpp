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
	// std::string s;
	if ((dir = opendir(_client._request.path.c_str())) == NULL)
	{
		_client._response.status = 404;
		perror("opendir() error");
		// std::cerr << "olAAAAA comistasss" << std::endl;
	}
	else
	{
		struct dirent *entry = readdir(dir);
		std::string _name;
		_client._request.body = "<html>\n"
						"<head><title>Index of " + _client._request.path + "</title></head>\n"
						"<body>\n";

		while ((entry = readdir(dir)) != NULL)
		{
			std::string data_name = entry->d_name;
			if (entry->d_type == DT_DIR)
				_client._request.body += "<h4><a href=\""+data_name+"/\">"+data_name+"</a><br/></h4>\n";
			else
				_client._request.body += "<h4><a href=\""+data_name+"\">"+data_name+"</a><br/></h4>\n";
		}
		closedir(dir);
	}
	_client._response.body = _client._request.body;






	// DIR *dir = opendir(_client._request.path.c_str());
    // // if (!dir)
    // // {
    // //     Serve.it = get_code("404");
    // //     Serve.path = serverI.error_pages[404];
    // //     return false;
    // // }
    // struct dirent *entry = readdir(dir);
    // _client._request.body = "<html>\n"
    //                   "<head><title>Index of " + _client._request.path + "</title></head>\n"
    //                   "<body>\n";
    // std::string name;
    // _client._request.body.append("<h1>Index Of The Directory</h1><hr><pre>\n");
    // while (entry != NULL)
    // {
    //     name = entry.d_name;
    //     // std::cout << "name :" << name << std::endl;
    //     if (entry.d_type == DT_DIR)
    //     {
    //         name.append("/");
    //         _client._request.body += "<a href=\"" + name + "\">" + name + "</a>\n";
    //     }
    //     else if (entry.d_type == DT_REG) // file
    //     {
    //         _client._request.body += "<a href=\"" + name + "\">" + name + "</a>\n";
    //     }
    //     _client._request.body += "<br>";
    //     entry = readdir(dir);
    // }
	// _client._response.body = _client._request.body;
	// std::cerr << "Ayoo: " << _client._request.body << std::endl;
}

void	_file_or_dir(  Client & _client )
{
	struct stat info;

	// std::cerr << "lpath lmrid: " << _client._request.path << std::endl;
	// std::cerr << "-----STATUS: " << _client._response.status << std::endl;
	if (stat(_client._request.path.c_str(), &info) != 0)
		_client._response.status = 404;

    if (S_ISDIR(info.st_mode))
		_client._request.type = "directory";
	else if (S_ISREG(info.st_mode))
		_client._request.type = "file";
	else
		_client._response.status = 404;
	// std::cerr << "STATUS: " << _client._response.status << std::endl;
}

void _body_parser(  Client & _client )
{
	std::map<std::string, std::string> m;

	std::vector<std::string> _req;
	std::string delimiter = "\r\n\r\n";
    size_t pos = _client._request.body.find(delimiter);
    std::string header = _client._request.body.substr(0, pos);
	// std::cerr << "kayyn: " << header << std::endl;
	_client._request.upload_data = "";
	// std::cerr << "body:\n" << _client._request.body << std::endl;
	for (size_t i=pos+4; i < _client._request.body.size(); i++)
	{	_client._request.upload_data += _client._request.body[i];
	}
	// std::cerr << "yo upload: " << _client._request.body.size() << std::endl;

	size_t boundary_pos = _client._request.headers["Content-Type"].find("boundary=")+9;
	_client._request.boundary = _client._request.headers["Content-Type"].substr(boundary_pos);

    int psps= _client._request.upload_data.find(_client._request.boundary);
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
	// std::cerr << "Content-Disposition: " << m["Content-Disposition"] << std::endl;
	if (pos)
	{
		std::vector<std::string> v;
		std::istringstream iss(m["Content-Disposition"]);
		std::string token;

		while (std::getline(iss, token, ';')) {
			v.push_back(token);
		}
		if (v.size() == 3)
		{
			size_t _p = v[1].find("\"");

			_client._request.upload_name = v[1].substr(_p+1, v[1].size()-_p-2);
			_p = v[2].find("\"");
			_client._request.upload_file_name = v[2].substr(_p+1, v[2].size()-_p-2);
		}
	}
	_client._request.upload_content_type = m["Content-Type"];

	// std::cerr << "upload shit:" << std::endl;
	// std::cerr << "upload_name: " << _client._request.upload_name << std::endl;
	// std::cerr << "upload_file_name: " << _client._request.upload_file_name << std::endl;
	// std::cerr << "upload_content_type: " << _client._request.upload_content_type << std::endl;
	// std::cerr << "data: " << _client._request.upload_data << std::endl;
}

void	_get( Client & _client, Server &_server )
{
	(void)_server;
	// std::ifstream _file;
	// int ff = open(_client._request.path);
	// if (_file)
	// {
	// 	_get_res_body(_client._request, _client._response);
	// }
	// else
	// {
	// 	std::cerr << "ppppppppppppp" << _client._request.path << std::endl;
	// 	_client._response.status = 404;
	// }
	// close(ff);

    _file_or_dir(_client);
	// std::cerr << "---------------------------" << std:: endl;

	std::cout << "GET" << _client._response.status << std::endl;

	if (!_client._response.status)
	{
		if (_client._request.type == "directory")
		{
			if (_client._request.path[_client._request.path.size()-1] != '/')
			{
				_client._request.path+='/';
				_client._response.status = 301;
				_client._response.location = _client._request.uri+'/';
				// std::cerr << "iiiii: " << _client._request.path << std::endl;
				// _get( _client._response, _client._request, _server );
			}
			else
			{
				if (_client._request.index.size())
				{
					if (_client._request.cgi.size())
						_cgi(_client, _server);
					else
						get_indexed_file_data(_client._request, _client._response, _client._request.path);
					_client._response.status = 200;
				}
				else
				{
					// autoindex
					if (!_client._request.autoindex)
						_client._response.status = 403;
					else
					{
						_get_listed_dir(_client);
						_client._response.body = _client._request.body;
						// std::cerr << "khsk tkni hna " << _client._request.path << " : " << _client._response.body << std::endl;
						_client._response.content_type = "text/html";
						_client._response.status = 200;
					}
				}
			}
		}
		else if (_client._request.type == "file")
		{
			// std::cerr << "path: " << _client._request.uri << std::endl;
			if (_client._request.cgi.size())
				_cgi(_client, _server);
			else
				_get_res_body(_client);
			_client._response.status = 200;
		}
		// else
		// 	_client._response.status = 404;
	}
}


void _post(  Client & _client , Server &_server )
{
	std::cout << "POST" << std::endl;

	if (!_client._response.status)
	{
		// if (_client._request.headers["Content-Type"].substr(0, 19) == "multipart/form-data")
		if (_client._request.upload_path.size())
		{
			// Upload the shit
			// _body_parser(_client._request);
			
			_client._response.content_length = _client._request.body.size();//_client._request.upload_data.size();
			_client._response.content_type = "text/html";//_client._request.upload_content_type;
			
			// std::cerr << "multipart shit: " << _client._request.upload_file_name << std::endl;
			// creating the file
			
			// fill it
			_cgi(_client, _server);
			if (!_client._response.body.size())
			{
				// gg
				_body_parser(_client);
				
				std::ofstream _upload_file(_client._request.path+'/'+_client._request.upload_file_name);
				
				_upload_file << _client._request.upload_data;
				// _client._response.content_type = _client._request.upload_content_type;
				_client._response.content_type = _client._response.mims[_get_ex(_client._request.upload_file_name)];
				_client._response.body = _client._request.body;
				_client._response.content_length = _client._response.body.size();
				// std::cerr << "file size: " << _client._response.body.size() << std::endl;
				// std::cerr << "wa lwzz: " << _client._response.content_type << std::endl;
				// std::cerr << "gg: " << _client._response.content_type << std::endl;
			}
			_client._response.status = 200;
		}
		else
		{
			// std::ifstream _file;
			// _file.open(_client._request.path);
			// _file ? _get_res_body(_client._request, _client._response) : _client._response.status = 404;

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
							// if (!_client._response.body.size())
								// run its source code
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
					// if (!_client._response.body.size())
						// run its source code
				else
				{
					_client._response.status = 200;
					_get_res_body(_client);
				}
			}
			// else
			// 	_client._response.status = 404;
		}
	}
}

void _delete( Client & _client , Server &_server )
{
	std::cout << "DELETE" << std::endl;

	// std::ifstream _file;
	// _file ? _get_res_body(_request, _response) : _response->status = 404;

    _file_or_dir(_client);
	std::cerr << "--------_request->type : " << _client._request.type << std::endl;
	if (_client._request.type == "directory")
	{
		if (_client._request.path[_client._request.path.size()-1] != '/')
			_client._response.status = 409;//Conflict
		else
		{
			if (rmdir(_client._request.path.c_str()) != 0) {
				perror("Error deleting the directory");
				if (!access(_client._request.path.c_str(), W_OK))
					_client._response.status = 500;// Internal Server Error
				else
					_client._response.status = 403;// Forbidden
			}
			else
				_client._response.status = 204;// No Content
		}
	}
	else if (_client._request.type == "file")
	{

		_client._response.status = 204;// No Content
		std::cerr << "delete path : " << _client._request.path << std::endl;
		if (std::remove(_client._request.path.c_str()) != 0) {
			perror("Error deleting the file");
		}
	}
}
