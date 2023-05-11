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

int _get_matched_location( t_server *_server, Request *_request )
{
    for (size_t i=0; i < _server->location.size(); i++)
        if (_server->location[i] == _request->uri)
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

void	_validate_request( std::map<std::string, std::string> m, t_server *_server, Response *_response, Request *_request )
{
    std::map<std::string, std::string>::iterator _transfer_encoding_iter = m.find("Transfer-Ecoding");
	std::map<std::string, std::string>::iterator _content_length_iter = m.find("Content-Length");
	std::map<std::string, std::string>::iterator _req_body_iter = m.find("body");

	if (_transfer_encoding_iter != m.end() && _transfer_encoding_iter->second != "chunked")
		_response->status = 501;
	if (_transfer_encoding_iter == m.end() && _content_length_iter == m.end() && _request->method == "Post")
		_response->status = 400;
	if (!_valid_url_chars(_request->uri))
		_response->status = 400;
	if ((_request->uri).size() > 2048)
		_response->status = 414;
	if (static_cast<int>(_req_body_iter->second.size()) > _server->max_body_size)
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

void _fill_request(t_server *_server, Request *_request )
{
	_request->index = _server->index;
	_server->http_redirection.size() ? _request->redirection = _server->http_redirection :  _request->redirection = "";
	
	// hadi ghi binmaa wsf
	if (_request->uri == "/")
		_request->path = _server->root+_request->uri+"index.html";
	else
		_request->path = _server->root+_request->uri;
    _server->cgi.size() ? _request->cgi = _server->cgi :  _request->cgi = "";
}

void	_request( t_server *_server, Request *_request, Response *_response, char *s )
{
    std::map<std::string, std::string> m;
    std::vector<std::string> _req;
    char *p;

    // Here, the delimiter is white space.
    p = strtok(s, "\n");
    while (p != NULL) {
        _req.push_back(std::string(p));
        p = strtok(NULL, "\n");
    }




    // extract the method, the uri and the http-version
    _extract_first_line(_request, &_req[0][0]);
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
                m.insert(std::make_pair(_key, _value));
                _key = "";
                _value = "";
            }
            t = strtok(NULL, ":");
            y++;
        }
    }
	_fill_request(_server, _request);
	// _request->index = _server->index;
	// _server->http_redirection.size() ? _request->redirection = _server->http_redirection :  _request->redirection = "";

    // std::map<std::string, std::string>::iterator iter;
    // for (iter = m.begin(); iter != m.end(); iter++)
    // {
    //   std::cout<<(*iter).first<<"   ---   "<<(*iter).second<<"\n";
    // }
	// Responseponse *_response;
    _validate_request(m, _server, _response, _request);
}
