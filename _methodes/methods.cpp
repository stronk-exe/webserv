/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:15:32 by ael-asri          #+#    #+#             */
/*   Updated: 2023/06/22 18:46:40 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

void print_error(std::string s)
{
	std::cerr << s << std::endl;
    exit(1);
}

void	_get_listed_dir( Request *_request, Response *_response )
{
	DIR *dir;
	// std::string s;
	if ((dir = opendir(_request->path.c_str())) == NULL)
	{
		_response->status = 404;
		perror("opendir() error");
	}
	else
	{
		struct dirent *entry = readdir(dir);
		std::string _name;
		_request->body = "<html>\n"
						"<head><title>Index of " + _request->path + "</title></head>\n"
						"<body>\n";

		while ((entry = readdir(dir)) != NULL)
		{
			std::string data_name = entry->d_name;
			if (entry->d_type == DT_DIR)
				_request->body += "<h4><a href=\""+data_name+"/\">"+data_name+"</a><br/></h4>\n";
			else
				_request->body += "<h4><a href=\""+data_name+"\">"+data_name+"</a><br/></h4>\n";
		}
		closedir(dir);
	}
	_response->body = _request->body;






	// DIR *dir = opendir(_request->path.c_str());
    // // if (!dir)
    // // {
    // //     Serve.it = get_code("404");
    // //     Serve.path = serverI->error_pages[404];
    // //     return false;
    // // }
    // struct dirent *entry = readdir(dir);
    // _request->body = "<html>\n"
    //                   "<head><title>Index of " + _request->path + "</title></head>\n"
    //                   "<body>\n";
    // std::string name;
    // _request->body.append("<h1>Index Of The Directory</h1><hr><pre>\n");
    // while (entry != NULL)
    // {
    //     name = entry->d_name;
    //     // std::cout << "name :" << name << std::endl;
    //     if (entry->d_type == DT_DIR)
    //     {
    //         name.append("/");
    //         _request->body += "<a href=\"" + name + "\">" + name + "</a>\n";
    //     }
    //     else if (entry->d_type == DT_REG) // file
    //     {
    //         _request->body += "<a href=\"" + name + "\">" + name + "</a>\n";
    //     }
    //     _request->body += "<br>";
    //     entry = readdir(dir);
    // }
	// _response->body = _request->body;
	// std::cerr << "Ayoo: " << _request->body << std::endl;
}

void	_file_or_dir( Request *_request, Response *_response )
{
	struct stat info;

	// std::cerr << "lpath lmrid: " << _request->path << std::endl;
	if (stat(_request->path.c_str(), &info) != 0)
		_response->status = 404;
	
	std::cerr  << "_response->status : "  << _response->status << std::endl;
    if (S_ISDIR(info.st_mode))
		_request->type = "directory";
	else if (S_ISREG(info.st_mode))
		_request->type = "file";
	else
		_response->status = 404;
}

void _body_parser( Request *_request )
{
	std::map<std::string, std::string> m;

	std::vector<std::string> _req;
	std::string delimiter = "\r\n\r\n";
    size_t pos = _request->body.find(delimiter);
    std::string header = _request->body.substr(0, pos);
	// std::cerr << "kayyn: " << header << std::endl;
	_request->upload_data = "";
	// std::cerr << "body:\n" << _request->body << std::endl;
	for (size_t i=pos+4; i < _request->body.size(); i++)
	{	_request->upload_data += _request->body[i];
	}
	// std::cerr << "yo upload: " << _request->body.size() << std::endl;

	size_t boundary_pos = _request->headers["Content-Type"].find("boundary=")+9;
	_request->boundary = _request->headers["Content-Type"].substr(boundary_pos);

    int psps= _request->upload_data.find(_request->boundary);
	std::string _real_data = "";

	for (size_t i=0; i < _request->upload_data.size() && i < psps-4; i++)
		_real_data += _request->upload_data[i];
	_request->upload_data = _real_data;

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

			_request->upload_name = v[1].substr(_p+1, v[1].size()-_p-2);
			_p = v[2].find("\"");
			_request->upload_file_name = v[2].substr(_p+1, v[2].size()-_p-2);
		}
	}
	_request->upload_content_type = m["Content-Type"];

	// std::cerr << "upload shit:" << std::endl;
	// std::cerr << "upload_name: " << _request->upload_name << std::endl;
	// std::cerr << "upload_file_name: " << _request->upload_file_name << std::endl;
	// std::cerr << "upload_content_type: " << _request->upload_content_type << std::endl;
	// std::cerr << "data: " << _request->upload_data << std::endl;
}

void	_get( Response *_response, Request *_request, Server &_server )
{
	(void)_server;
	std::ifstream _file;
	std::cerr << "_request->path : " << _request->path << std::endl;
	_file ? _get_res_body(_request, _response) : _response->status = 404;

    _file_or_dir(_request, _response);

	std::cout << "GET" << _response->status << std::endl;

	if (!_response->status)
	{
		if (_request->type == "directory")
		{
			if (_request->path[_request->path.size()-1] != '/')
			{
				_request->path+='/';
				_response->status = 301;
				_response->location = _request->uri+'/';
				// std::cerr << "iiiii: " << _request->path << std::endl;
				// _get( _response, _request, _server );
			}
			else
			{
				if (_request->index.size())
				{
					if (_request->cgi.size())
						_cgi(_request, _response, _server);
					else
						get_indexed_file_data(_request, _response, _request->path);
					_response->status = 200;
				}
				else
				{
					// autoindex
					if (!_request->autoindex)
						_response->status = 403;
					else
					{
						_get_listed_dir(_request, _response);
						_response->body = _request->body;
						// std::cerr << "khsk tkni hna " << _request->path << " : " << _response->body << std::endl;
						_response->content_type = "text/html";
						_response->status = 200;
					}
				}
			}
		}
		else if (_request->type == "file")
		{
			std::cerr << "path: " << _request->uri << std::endl;
			if (_request->cgi.size())
				_cgi(_request, _response, _server);
			else
				_get_res_body(_request, _response);
			_response->status = 200;
		}
	}
}


void _post( Response *_response, Request *_request, Server &_server )
{
	std::cout << "POST" << std::endl;

	if (!_response->status)
	{
		// if (_request->headers["Content-Type"].substr(0, 19) == "multipart/form-data")
		if (_request->upload_path.size())
		{
			// Upload the shit
			// _body_parser(_request);
			
			_response->content_length = _request->body.size();//_request->upload_data.size();
			_response->content_type = "text/html";//_request->upload_content_type;
			
			// std::cerr << "multipart shit: " << _request->upload_file_name << std::endl;
			// creating the file
			
			// fill it
			_cgi(_request, _response, _server);
			if (!_response->body.size())
			{
				// gg
				_body_parser(_request);
				
				std::ofstream _upload_file(_request->path+'/'+_request->upload_file_name);
				
				_upload_file << _request->upload_data;
				// _response->content_type = _request->upload_content_type;
				_response->content_type = _response->mims[_get_ex(_request->upload_file_name)];
				_response->body = _request->body;
				_response->content_length = _response->body.size();
				std::cerr << "file size: " << _response->body.size() << std::endl;
				// std::cerr << "wa lwzz: " << _response->content_type << std::endl;
				// std::cerr << "gg: " << _response->content_type << std::endl;
			}
			_response->status = 200;
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
							_cgi(_request, _response, _server);
							// if (!_response->body.size())
								// run its source code
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
					_cgi(_request, _response, _server);
					// if (!_response->body.size())
						// run its source code
				else
				{
					_response->status = 200;
					_get_res_body(_request, _response);
				}
			}
		}
	}
}

void _delete(  Response *_response, Request *_request ,Server &_server )
{
	std::cout << "DELETE" << std::endl;

	std::ifstream _file;
	_file ? _get_res_body(_request, _response) : _response->status = 404;

    _file_or_dir(_request, _response);
	std::cerr << "--------_request->type : " << _request->type << std::endl;
	if (_request->type == "directory")
	{
		if (_request->path[_request->path.size()-1] != '/')
			_response->status = 409;//Conflict
		else
		{
			if (rmdir(_request->path.c_str()) != 0) {
				perror("Error deleting the directory");
				if (!access(_request->path.c_str(), W_OK))
					_response->status = 500;// Internal Server Error
				else
					_response->status = 403;// Forbidden
			}
			else
				_response->status = 204;// No Content
		}
	}
	else if (_request->type == "file")
	{

		_response->status = 204;// No Content
		std::cerr << "delete path : " << _request->path << std::endl;
		if (std::remove(_request->path.c_str()) != 0) {
			perror("Error deleting the file");
		}
	}
}

