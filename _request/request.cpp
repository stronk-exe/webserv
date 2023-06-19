/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:15:39 by ael-asri          #+#    #+#             */
/*   Updated: 2023/06/11 20:37:33 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

int	_max_element( std::vector<int> v )
{
	int	temp = -999;
	int	index = 0;
	for (size_t i=0; i <v.size(); i++)
		if (temp < v[i])
		{
			temp = v[i];
			index = i;
		}
	return index;
}

int _match_theLocation( Server &_server, Location &_location, Request *_request )
{
	std::vector<int> v;
	int count;

	for (size_t i=0; i < _server.locations.size(); i++)
	{
		count = 0;
		for (size_t j=0; j<_server.locations[i].name.size() && j<_request->uri.size() && _server.locations[i].name[j] == _request->uri[j]; j++)
			count++;
		v.push_back(count);
	}
	if (!v.size())
		return 0;
	_location = _server.locations[_max_element(v)];
	return 1;
}

int _valid_url_chars( std::string s )
{
    std::string t = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	for (size_t i=0; i<s.size(); i++)
		if (t.find(s[i])<0)
			return 0;
    return 1;
}

int	_is_method_allowed( Location _location, Request *_request )
{
	for (size_t i=0 ; i<_location.allows_methods.size(); i++)
		if (_location.allows_methods[i] == _request->method)
			return 1;
	return 0;
}

void	_validate_request( Server &_server, Location &_location, Request *_request, Response *_response )
{
    std::map<std::string, std::string>::iterator _transfer_encoding_iter = _request->headers.find("Transfer-Ecoding");
	std::map<std::string, std::string>::iterator _content_length_iter = _request->headers.find("Content-Length");

	if (_transfer_encoding_iter != _request->headers.end() && _transfer_encoding_iter->second != "chunked")
		_response->status = 501;
	if (_transfer_encoding_iter == _request->headers.end() && _content_length_iter == _request->headers.end() && _request->method == "POST")
		_response->status = 400;
	if (!_valid_url_chars(_request->uri))
		_response->status = 400;
	if ((_request->uri).size() > 2048)
		_response->status = 414;
	if (static_cast<int>(_request->body.size()) > (str_to_num(_server.client_max_body_size)* 1e6) )
		_response->status = 413;
	if (_is_method_allowed(_location, _request))
		_request->is_method_allowed = 1;
	if (!_match_theLocation(_server, _location, _request))
		_response->status = 404;
	if (_request->redirection.size())
	{
		_response->path = _request->redirection[0];
		_response->status = 301;
	}
}

void _extract_first_line( Request *_request, std::string s )
{
    std::vector<std::string> v;
    char *_line = strtok(&s[0], " ");

    while (_line != NULL)
    {
        v.push_back(_line);
        _line = strtok(NULL, " ");
    }
    if (v.size() == 3)
    {
        _request->method = v[0];
        _request->uri = v[1];
    }
    // else
	// 	print_error("missing method or uri or http-version here!");
}

void _fill_request(Server &_server, Location &_location, Request *_request )
{
	if (_location.index.size())
		for (size_t i=0; i<_location.index.size(); i++)
			_request->index.push_back(_location.index[i]);
	if (_server.index.size())
		for (size_t i=0; i<_server.index.size(); i++)
			_request->index.push_back(_server.index[i]);
	_location.autoindex ? _request->autoindex = 1 : _request->autoindex = 0;
	_request->root = _location.root_location;
	_request->path = _request->root+_request->uri;
	if (_server.redirection.path.size())
		_request->redirection.push_back(_server.redirection.path);

	if (_location.cgi_pass.size())
		for (size_t i=0; i<_location.cgi_pass.size(); i++)
			_request->cgi.push_back(_location.cgi_pass[i]);
	
	if (_server.errors.size())
		for (size_t i=0; i<_server.errors.size(); i++)
			_request->error_pages.push_back(_server.errors[i]);
}

void _match_theServer( Parsing &_server, Request *_request, Server &_s)
{
    for (size_t i=0; i<_server.servers.size(); i++)
        if (_server.servers[i].name == _request->headers["Host"])
            _s =  _server.servers[i];
}

void	_request_parser( Request *_request, std::string r )
{
	std::vector<std::string> _req;
	std::string delimiter = "\r\n\r\n";
    size_t pos = r.find(delimiter);
    std::string header = r.substr(0, pos);

	_request->body = "";
	for (size_t i=pos+4; i < r.size(); i++)
		_request->body += r[i];

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

void	_complete_body_filling( Request *_request )
{
	std::cerr << "wew wew: " << str_to_num(_request->headers["Content-Length"].substr(0, _request->headers["Content-Length"].size())) << std::endl;
	if (str_to_num(_request->headers["Content-Length"].substr(0, _request->headers["Content-Length"].size())) > _request->body.size())
	{
		while (str_to_num(_request->headers["Content-Length"].substr(0, _request->headers["Content-Length"].size())) > _request->body.size())
		{
			char buffer[999999] = {0};
			int data = read(_request->fd, buffer, 999999);
			if (data < 0)
				print_error("empty data!");
			for (int i=0; i<data; i++)
				_request->body += buffer[i];
		}
	}
}

void	_get_mims( Response *_response )
{
	std::vector<std::string> v;
	std::string key, value;

    std::ifstream file("/Users/ael-asri/Desktop/wipsirv/_request/mims");

    if (file.is_open())
	{
        std::string line;
        while (std::getline(file, line))
            v.push_back(line);
        file.close();
	}
	
	for (size_t i=0; i < v.size(); i++)
	{
		int pos = v[i].find(" ");
		if (pos != std::string::npos)
		{
			key = v[i].substr(0, pos);
			value = v[i].substr(pos + 1);
			_response->mims[key] = value;
		}
	}
}

void	_request( Parsing &_server, Server &_s, Request *_request, Response *_response, std::string s )
{
	// Server _s;
	Location _location;
	
	_request_parser(_request, s);

	// if the body is not complete yet
	// _complete_body_filling(_request);
	
	
    _match_theServer(_server, _request, _s);
	_match_theLocation(_s, _location, _request);
	_fill_request(_s, _location, _request);
    _validate_request(_s, _location, _request, _response);
	_get_mims(_response);


	// std::cerr << "buffer: " << s << std::endl;
	// std::map<std::string, std::string>::iterator iter;
    // for (iter = _response->mims.begin(); iter != _response->mims.end(); iter++)
    // {
    //     std::cout << "{" << (*iter).first << "}---{" << (*iter).second << "}" << std::endl;
    // }
	// std::cout << "request body: " << _request->body << std::endl;
}
