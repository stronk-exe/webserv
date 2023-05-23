/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-asri <ael-asri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:15:39 by ael-asri          #+#    #+#             */
/*   Updated: 2023/05/11 11:58:11 by ael-asri         ###   ########.fr       */
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
    // std::cerr << "wwww" << std::endl;
	_location = _server.locations[_max_element(v)];
	return 1;
}

int _valid_url_chars( std::string s )
{
    std::string t = "ABCDEFGHIJKLMNOPQRSTUVWXYZazcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	for (int i=0; s[i]; i++)
		if (!t.find(s[i]))
			return 0;
    return 1;
}

void	_validate_request( Server &_server, Location &_location, Request *_request, Response *_response )
{
    std::map<std::string, std::string>::iterator _transfer_encoding_iter = _request->headers.find("Transfer-Ecoding");
	std::map<std::string, std::string>::iterator _content_length_iter = _request->headers.find("Content-Length");
	// std::map<std::string, std::string>::iterator _req_body_iter = _request->headers.find("body");

	if (_transfer_encoding_iter != _request->headers.end() && _transfer_encoding_iter->second != "chunked")
		_response->status = 501;
	if (_transfer_encoding_iter == _request->headers.end() && _content_length_iter == _request->headers.end() && _request->method == "POST")
		_response->status = 400;
	if (!_valid_url_chars(_request->uri))
		_response->status = 400;
	if ((_request->uri).size() > 2048)
		_response->status = 414;
	if (static_cast<int>(_request->body.size()) > str_to_num(_server.client_max_body_size) )
		_response->status = 413;
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
    else
		print_error("missing method or uri or http-version here!");
}

// void _match_theLocation( Server &_server, Request *_request, Location &_location )
// {
//     for (size_t i=0; i < _server.locations.size(); i++)
//         if (_server.locations[i].name == _request->uri)
//             _location =  _server.locations[i];
// }

void _fill_request(Server &_server, Location &_location, Request *_request )
{
	// Location _location;

	// match the location
	// std::cerr << "server: " << _server.name << std::endl;
	// _match_theLocation(_server, _location, _request);

    // - Fill the index
	// std::cerr << "sbah hada:" << _location.index.size() << " - " << _location.autoindex << std::endl;
	// std::cerr << "req parsed uri:" << _request->uri <<  "| path: "<< _request->path<< " | root: " << _request->root << std::endl;
	if (_location.index.size())
	{
		// _request->index = _location.index;
		for (size_t i=0; i<_location.index.size(); i++)
			_request->index.push_back(_location.index[i]);
	}
	if (_server.index.size())
	{
		// _request->index = _server.index;
		for (size_t i=0; i<_server.index.size(); i++)
			_request->index.push_back(_server.index[i]);
	}
	_location.autoindex ? _request->autoindex = 1 : _request->autoindex = 0;
	// for (size_t i=0; i<_server.locations.size(); i++)
	// {
	// 	for (size_t i=0; i<_server.index.size(); i++)
	// 		_request->index.push_back(_server.index[i]);
	// }
	// std::cerr << "root: " << _location.root_location << " : " << _location.name << std::endl;
	_request->root = _location.root_location;
	_request->path = _request->root+_request->uri;
    // std::cerr << "chihaja hnaya :" << _request->path << "~" << _request->root << "~" << _request->uri << std::endl;
	// std::cerr << "Ayoo: " << _server.redirection.path.size() << std::endl;
	_server.redirection.path.size() ? _request->redirection.push_back(_server.redirection.path) :  _request->redirection.push_back("");

	// if (_location.index.size())
	// {
	// 	// which index ??? .html or .php or what
	// 	// _request->index = _location.index;
	// 	for (size_t i=0; _location.index.size(); i++)
	// 		_request->index.push_back(_location.index[i]);
	// }
	// if (_server.index.size())
	// {
	// 	for (size_t i=0; _server.index.size(); i++)
	// 		_request->index.push_back(_server.index[i]);
	// }
	// if (_request->uri == "/")
	// 	_request->path = _server.servers[0].locations[0].name+_request->uri+"index.html";
	// else
		// _request->path = _server.servers[0].locations[0].name+_request->uri;
	if (_location.cgi_pass.size())
		for (size_t i=0; i<_location.cgi_pass.size(); i++)
			_request->cgi.push_back(_location.cgi_pass[i]);
	
	if (_server.errors.size())
	{
		for (size_t i=0; i<_server.errors.size(); i++)
		{
			// _request->error_pages.status = _server.errors[i].error_status;
			// _request->error_pages.path = _server.errors[i].path;
			_request->error_pages.push_back(_server.errors[i]);
			std::cerr << "error_pages: " << _request->error_pages[i].path << std::endl;
		}
	}
}


// void _requset_parser( Request *_request, char *s )
// {
//     std::string delimiter = "\r\n\r\n";
//     std::string r = s;
//     size_t pos = r.find(delimiter);
//     std::string headersStr = r.substr(0, pos);
//     _request->body = r.substr(pos + delimiter.length());

//     std::cerr << "poos: " << pos << std::endl;

//     // Split the headers into individual lines
//     std::vector<std::string> headerLines;
//     delimiter = "\r\n";
//     pos = 0;
//     std::string line;
//     while ((pos = headersStr.find(delimiter)) != std::string::npos) {
//         line = headersStr.substr(0, pos);
//         headerLines.push_back(line);
//         headersStr.erase(0, pos + delimiter.length());
//     }
//     headerLines.push_back(headersStr);

//     // Parse the headers into key-value pairs
//     std::string key, value;
//     // for (std::string headerLine : headerLines)
//     for (size_t i=0; i < headerLines.size(); i++)
//     {
//         pos = headerLines[i].find(":");
//         if (pos != std::string::npos) {
//             key = headerLines[i].substr(0, pos);
//             value = headerLines[i].substr(pos + 2);
//             _request->headers[key] = value;
//         }
//     }
// }

// std::string _str_trim( std::string s )
// {
//     std::cerr << "s: {" << s << "}" << std::endl;
//     size_t i=0;

//     for (; s.size() && s[i] == ' '; i++)
//         s.erase(s.begin()+i);
//     for (; s.size() && s[i] != ' ' && s[i] != '\n' && s[i] != '\r'; i++)
//         ;
//     for (; i<s.size(); i++)
//         s.erase(s.begin()+i);
//     // s.erase(s.begin()+s.size());
//     std::cerr << "s: {" << s << "}" << std::endl;
//     return s;
// }

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
    // std::string r = s;
    size_t pos = r.find(delimiter);
    std::string header = r.substr(0, pos);
	// std::cerr << r.size() << " ======================\n";
	_request->body = "";
	for (size_t i=pos+4; i < r.size(); i++)
	{
		// std::cerr << r[i];
		_request->body += r[i];
	}
	// std::cerr << "======================\n";
    // _request->body = strdup((r.substr(pos + delimiter.length())).c_str());
	// std::cerr << "boddddy:" << r << std::endl;

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

void	_request( Parsing &_server, Server &_s, Request *_request, Response *_response, std::string s )
{
	// Server _s;
	Location _location;
	
	_request_parser(_request, s);

	
	std::cerr << "content length: " << str_to_num(_request->headers["Content-Length"].substr(0, _request->headers["Content-Length"].size())) << ">-<" << _request->body.size() << std::endl;
	if (str_to_num(_request->headers["Content-Length"].substr(0, _request->headers["Content-Length"].size())) > _request->body.size())
	{
		while (str_to_num(_request->headers["Content-Length"].substr(0, _request->headers["Content-Length"].size())) > _request->body.size())
		{
			char buffer[999999] = {0};
			int data = read(_request->fd, buffer, 999999);
			if (data < 0)
				print_error("empty data!");
			// std::string _test_buffer;
			for (int i=0; i<data; i++)
				_request->body += buffer[i];
			// _request->headers["Content-Length"] += data;
			std::cerr << "content length: " << _request->headers["Content-Length"] << ">-<" << _request->body.size() << std::endl;
		}
	}
    _match_theServer(_server, _request, _s);
	_match_theLocation(_s, _location, _request);
	_fill_request(_s, _location, _request);
	std::cerr << "req parsed uri:" << _request->uri <<  "| path: "<< _request->path<< " | root: " << _request->root << std::endl;
    _validate_request(_s, _location, _request, _response);

	// std::cerr << "path: " << _request->path << std::endl;


 /* 	std::cerr << "request parsed headers: " << std::endl;
    std::map<std::string, std::string>::iterator iter;
    for (iter = _request->headers.begin(); iter != _request->headers.end(); iter++)
    {
        std::cout << "{" << (*iter).first << "}---{" << (*iter).second << "}" << std::endl;
    }
    std::cerr << "request parsed body: " << std::endl;
    std::cerr << _request->body << std::endl;
	exit(1);*/
}
