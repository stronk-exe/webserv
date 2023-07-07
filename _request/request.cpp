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

int _match_theLocation( Server &_server, Location &_location, Request &_request )
{
	std::vector<int> v;
	int count;

	for (size_t i=0; i < _server.locations.size(); i++)
	{
		count = 0;
		for (size_t j=0; j<_server.locations[i].name.size() && j<_request.uri.size() && _server.locations[i].name[j] == _request.uri[j]; j++)
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

int	_is_method_allowed( Location _location, Request &_request )
{
	for (size_t i=0 ; i<_location.allows_methods.size(); i++)
	{
		// //std::cerr << "Ayoooo: " << _location.allows_methods[i] << std::endl;
		if (_location.allows_methods[i] == _request.method)
			return 1;
	}
	//std::cerr << "BZAF KAYTGHDAWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
	return 0;
}

void	_validate_request( Server &_server, Location &_location, Request &_request, Response &_response )
{
    std::map<std::string, std::string>::iterator _transfer_encoding_iter = _request.headers.find("Transfer-Ecoding");
	std::map<std::string, std::string>::iterator _content_length_iter = _request.headers.find("Content-Length");

	if (_transfer_encoding_iter != _request.headers.end() && _transfer_encoding_iter->second != "chunked")
		_response.status = 501;
	if (_transfer_encoding_iter == _request.headers.end() && _content_length_iter == _request.headers.end() && _request.method == "POST")
		_response.status = 400;
	if (!_valid_url_chars(_request.uri))
		_response.status = 400;
	if ((_request.uri).size() > 2048)
		_response.status = 414;
	if (static_cast<int>(_request.body.size()) > (str_to_num(_server.client_max_body_size)* 1e6) )
		_response.status = 413;
	if (_is_method_allowed(_location, _request))
		_request.is_method_allowed = 1;
	else
		_request.is_method_allowed = 0;
	if (!_match_theLocation(_server, _location, _request))
		_response.status = 404;
	if (_request.redirection.size())
	{
		_response.path = _request.redirection[0];
		_response.status = 301;
	}
}

void _extract_first_line( Request &_request, std::string s )
{
    std::vector<std::string> v;
    char *_line = strtok(&s[0], " ");

	//std::cerr << "STTTTRRRRRRRRRRRRRRRR: " << s << std::endl;
    while (_line != NULL)
    {
        v.push_back(_line);
        _line = strtok(NULL, " ");
    }
    if (v.size() == 3)
    {
        _request.method = v[0];
        _request.uri = v[1];
    }
    // else
	// 	print_error("missing method or uri or http-version here!");
}

// std::string	_get_Path( std::string path )
// {
// 	DIR *dir;
// 	struct dirent *entry;

// 	std::string s;

// 	if ((dir = opendir(path.c_str())) == NULL)
// 		perror("opendir() error");
// 	else
// 	{
// 		// puts("contents of root:");
// 		// while ((entry = readdir(dir)) != NULL)
// 		// {
// 		// 	std::string data_name = entry.d_name;
// 		// 	s += "<h3><a href="+data_name+">"+data_name+"</a><br/></h3>";
// 		// }
// 		closedir(dir);
// 	}
// }

void _fill_request(Server &_server, Location &_location, Request &_request )
{
	if (_location.index.size())
		for (size_t i=0; i<_location.index.size(); i++)
			_request.index.push_back(_location.index[i]);
	if (_server.index.size())
		for (size_t i=0; i<_server.index.size(); i++)
			_request.index.push_back(_server.index[i]);
	_location.autoindex ? _request.autoindex = 1 : _request.autoindex = 0;
	_request.root = _location.root_location;
	// if (_request.path == "/Users/ael-asri/Desktop/wipsirv/public")
	// 	_request.path += '/' + _request.root ;
	// if (_request.uri != "/favicon.ico")
	// _new_location = _get_Path( _request.uri );

	_request.path = _webserv_loc + "/public/" + _request.root + _request.uri;
	// //std::cerr << "PATH: " << _request.path << std::endl;
	if (_server.redirection.path.size())
		_request.redirection.push_back(_server.redirection.path);

	if (_location.cgi_pass.size())
		for (size_t i=0; i<_location.cgi_pass.size(); i++)
			_request.cgi.push_back(_location.cgi_pass[i]);
	
	if (_server.errors.size())
		for (size_t i=0; i<_server.errors.size(); i++)
			_request.error_pages.push_back(_server.errors[i]);
	_location.uploadDir.size() ? _request.upload_path = _location.uploadDir : _request.upload_path = "";
}

int	_match_theServer( Parsing &_server, Request &_request, Server &_s)
{
    for (size_t i=0; i<_server.servers.size(); i++)
    {
		// //std::cerr << "srvers names: " << _server.servers[i].name << " - " << _request.headers["Host"] << std::endl;
		if (_server.servers[i].name == _request.headers["Host"])
        {
			_s =  _server.servers[i];
			return 1;
		}
	}
	return 0;
}

int	_match_thePort( Parsing &_server, Request &_request, Server &_s)
{
	int	_pos = _request.uri.find(':');
	// //std::cerr << "Pos: " << _pos << std::endl;
	if (_pos >= 0)
	{
		std::string _port = _request.uri.substr(_pos+1, _request.uri.size());
		// //std::cerr << "Port: " << _port << std::endl;
		for (size_t i=0; i<_server.servers.size(); i++)
		{
			// //std::cerr << "srvers names: " << _server.servers[i].name << " - " << _request.headers["Host"] << std::endl;
			if (_server.servers[i].listen_port == str_to_num(_port))
			{
				_s =  _server.servers[i];
				return 1;
			}
		}

	}
	return 0;
}

void	_request_parser( Request &_request, std::string r )
{
	std::vector<std::string> _req;
	std::string delimiter = "\r\n\r\n";
    size_t pos = r.find(delimiter);
    std::string header = r.substr(0, pos);

	_request.body = "";
	for (size_t i=pos+4; i < r.size(); i++)
		_request.body += r[i];

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
	//std::cerr << "SIIIZEEEEEEEEEE: " << r << std::endl;
	_extract_first_line(_request, v[0]);
	//std::cerr << "MEEEEEETHHOOOOOD: " << _request.method << std::endl;

	std::string key, value;
	for (size_t i=0; i < v.size(); i++)
	{
		pos = v[i].find(":");
		if (pos != std::string::npos) {
			key = v[i].substr(0, pos);
			value = v[i].substr(pos + 2);
			_request.headers[key] = value;
		}
	}
}

// void	_complete_body_filling( Request *_request )
// {
// 	// //std::cerr << "wew wew: " << str_to_num(_request.headers["Content-Length"].substr(0, _request.headers["Content-Length"].size())) << " ~ " << _request.body.size() << std::endl;
// 	if (str_to_num(_request.headers["Content-Length"].substr(0, _request.headers["Content-Length"].size())) > _request.body.size())
// 	{
// 		while (str_to_num(_request.headers["Content-Length"].substr(0, _request.headers["Content-Length"].size())) > _request.body.size())
// 		{
// 			char buffer[999999] = {0};
// 			// //std::cerr << "sizooon: " << _request.fd << std::endl;
// 			// int newFd = dup(_request.fd);
// 			// // _request.fd = newFd;
// 			int data = read(_request.fd, buffer, 999999);
// 			// close(newFd);
// 			// int data = recv(_request.fd, buffer, 999999, 0);
// 			// //std::cerr << "sizzzzzzz" << std::endl;
// 			if (data < 0)
// 				print_error("empty data!");
// 			for (int i=0; i<data; i++)
// 				_request.body += buffer[i];
// 		}
// 	}
// }

void	_get_mims( Response &_response )
{
	std::vector<std::string> v;
	std::string key, value;

    std::ifstream file(( _webserv_loc + "/_request/mims"));

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
			_response.mims[key] = value;
		}
	}
}

std::string urlcode(const std::string& input) {
  std::ostringstream decoded;
  std::istringstream encoded(input);

  char ch;
  while (encoded.get(ch)) {
    if (ch == '%') {
      // Check for '%20' encoding
      if (encoded.peek() == '2' && (encoded.get(), encoded.peek() == '0')) {
        decoded << ' ';  // Decode space
        encoded.get();
      } else {
        // Decode other percent-encoded values
        int hex;
        if (!(encoded >> std::hex >> hex))
          break;
        decoded << static_cast<char>(hex);
      }
    } else {
      decoded << ch;
    }
  }

  return decoded.str();
}

void	_request( Parsing &_server, Server &_s, Request &_request, Response &_response, std::string s )
{
	// Server _s;
	Location _location;
	
	//std::cerr << "req Method: " <<s << std::endl;
	_request_parser(_request, s);
	// _request.uri = urlcode(_request.uri);
	// if the body is not complete yet
	// _complete_body_filling(_request);
	
	
    if (!_match_theServer(_server, _request, _s))
	{
		//go for ports;
		if (!_match_thePort(_server, _request, _s))
			_s = _server.servers[0];
	}
	_match_theLocation(_s, _location, _request);
	// //std::cerr << "location name: " << _location.name << std::endl;
	_fill_request(_s, _location, _request);
    _validate_request(_s, _location, _request, _response);
	//std::cerr << "BZAF KAYTGHDAQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ: " << _request.is_method_allowed << std::endl;
	_get_mims(_response);
	//std::cerr << "BZAF KAYTGHDAWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW: " << _request.is_method_allowed << std::endl;
	// //std::cerr << "_request name: " << _request.root << " - req uri: " << _request.uri << " - PATH: " << _request.path << std::endl;


	// //std::cerr << "buffer: " << s << std::endl;
	// std::map<std::string, std::string>::iterator iter;
    // for (iter = _response.mims.begin(); iter != _response.mims.end(); iter++)
    // {
    //     std::cout << "{" << (*iter).first << "}---{" << (*iter).second << "}" << std::endl;
    // }
	// std::cout << "request body: " << _request.body << std::endl;
}
