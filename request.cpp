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

#include "webserv.hpp"

int _get_matched_location( Server *_server, Request *_request )
{
    for (size_t i=0; i < _server->locations.size(); i++)
        if (_server->locations[i].name == _request->uri)
            return 1;
    return 0;
}

int _valid_url_chars( std::string s )
{
    std::string t = "ABCDEFGHIJKLMNOPQRSTUVWXYZazcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	for (int i=0; s[i]; i++)
		if (!t.find(s[i]))
			return 0;
    return 1;
}

void	_validate_request( Server *_server, Request *_request, Response *_response )
{
    std::map<std::string, std::string>::iterator _transfer_encoding_iter = _request->headers.find("Transfer-Ecoding");
	std::map<std::string, std::string>::iterator _content_length_iter = _request->headers.find("Content-Length");
	std::map<std::string, std::string>::iterator _req_body_iter = _request->headers.find("body");

	if (_transfer_encoding_iter != _request->headers.end() && _transfer_encoding_iter->second != "chunked")
		_response->status = 501;
	if (_transfer_encoding_iter == _request->headers.end() && _content_length_iter == _request->headers.end() && _request->method == "Post")
		_response->status = 400;
	if (!_valid_url_chars(_request->uri))
		_response->status = 400;
	if ((_request->uri).size() > 2048)
		_response->status = 414;
	if (static_cast<int>(_req_body_iter->second.size()) > /* max_body_size _server.servers[0].max_body_size here */98 )
		_response->status = 413;
	if (!_get_matched_location(_server, _request))
		_response->status = 404;
	if (_request->redirection.size())
	{
		_response->path = _request->redirection;
		_response->status = 301;
	}
}

void _extract_first_line( Request *_request, char *s )
{
    std::vector<std::string> v;
    char *_line = strtok(s, " ");
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

void _fill_request(Server _server, Request *_request )
{
    // - Fill the index
	// for (_server.index.size)
	// memcopy();
    // _request->index.insert(_request->index, _server->index, sizeof(_server->index) / sizeof(std::string));
	// std::cerr << "aaww: " << _server->index.size() << std::endl;
	// for (size_t i=0; i<_server->index.size(); i++)
	// 	std::cerr << _server->index[i] << std::endl;
	if (_server.index.size())
	{
		for (size_t i=0; i<_server.index.size(); i++)
			_request->index.push_back(_server.index[i]);
	}
	// for (size_t i=0; i<_server.locations.size(); i++)
	// {
	// 	for (size_t i=0; i<_server.index.size(); i++)
	// 		_request->index.push_back(_server.index[i]);
	// }
    
    // _server->http_redirection.size() ? _request->redirection = _server->http_redirection :  _request->redirection = "";
	
	// hadi ghi binmaa wsf
	// if (_request->uri == "/")
	// 	_request->path = _server.servers[0].locations[0].name+_request->uri+"index.html";
	// else
		// _request->path = _server.servers[0].locations[0].name+_request->uri;
    // _server.servers.locations[0].cgi_pass.size() ? _request->cgi = _server.servers[0].locations[0].cgi_pass :  _request->cgi = "";
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

Server	*_match_theServer( Parsing &_server, Request *_request)
{
	
    for (size_t i=0; i<_server.servers.size(); i++)
    {
		std::cerr << _server.servers[i].name << " - " << _request->headers["Host"] << std::endl;
        if (_server.servers[i].name == _request->headers["Host"])
		{
			Server s = _server.servers[i];
            _fill_request(s, _request);
            return &(_server.servers[i]);
		}
    }
	return NULL;
}

void	_request( Parsing &_server, Request *_request, Response *_response, char *s )
{
    // std::map<std::string, std::string> m;
    std::vector<std::string> _req;
    char *p;

    std::string delimiter = "\r\n\r\n";
    std::string r = s;
    size_t pos = r.find(delimiter);

    std::string header = r.substr(0, pos);
    _request->body = r.substr(pos + delimiter.length());
    // std::cerr << "raw req[0]: " << header << std::endl;
    // std::cerr << "raw req[1]: " << r.substr(pos + delimiter.length()) << std::endl;

    // Here, the delimiter is white space.
    p = strtok(&header[0], "\n");
    while (p != NULL)
    {
        _req.push_back(std::string(p));
        p = strtok(NULL, "\n");
    }

    // for (size_t i=0; i<_req.size(); i++)
    //     std::cerr << "headers req: " <<_req[i] << std::endl;
    // exit(1);

	

    // extract the method, the uri and the http-version
    _extract_first_line(_request, &_req[0][0]);
    // _requset_parser(_request, s);
    for (size_t x=1; x < _req.size(); x++)
    {
        char *r = &_req[x][0];
        char *t;

        // Here, the delimiter is white space.
        t = strtok(r, ":");
        int y=0;
        std::string _key, _value;
        while (t != NULL)
        {
            (!y%2) ? _key = t : _value = t;
            if (_key != "\0" && _value != "\0")
            {
                _request->headers.insert(std::make_pair(_key, _value));
                _key = "";
                _value = "";
            }
            t = strtok(NULL, ":");
            y++;
        }
    }

    // We must match the server first!
    Server *_s = _match_theServer(_server, _request);
	std::cerr << "aaww: " << _s->name << std::endl;
	if (s)
		_fill_request(*_s, _request);
	else
		print_error("ghayarha mamsalich lik db!");

	// _fill_request(_server, _request);
    std::cerr << "ha7na hna" << std::endl;
    // (void)_response;
    // exit(1);
	// _request->index = _server->index;
	// _server->http_redirection.size() ? _request->redirection = _server->http_redirection :  _request->redirection = "";

 /*   std::cerr << "request parsed headers: " << std::endl;
    std::map<std::string, std::string>::iterator iter;
    for (iter = _request->headers.begin(); iter != _request->headers.end(); iter++)
    {
        std::cout<<(*iter).first<<"   ---   "<<(*iter).second<<"\n";
    }
    std::cerr << "request parsed body: " << std::endl;
    std::cerr << _request->body << std::endl;*/
    // exit(1);
	// Responseponse *_response;
    _validate_request(_s, _request, _response);
}
