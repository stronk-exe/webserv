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
	{
		if (temp < v[i])
		{
			temp = v[i];
			index = i;
		}
	}
	return index;
}

void  removeLastWord(std::vector <std::string >& strs) {

    int pos0, pos1;
	// std::cerr << "####### " << str << std::endl;
	for (size_t t = 0; t < strs.size(); t++)
	{
		pos0 = 0;
		pos1 = 0;
		for (size_t k = strs[t].size(); k > 0; k--)
		{
			if (strs[t][k - 1] == '/')
			{
				if (!pos0)
					pos0 = k - 1;
				else if (!pos1)
					pos1 = k - 1;
			}
			if (pos0 && pos1)
				break;
		}
		// std::cerr << " pos0  " << pos0 << " - pos1 " << pos1 << std::endl;
		strs[t].erase(pos1 + 1, pos0);
		// std::cerr << " str : " << strs[t] << std::endl;
	}
}

bool sub_location(std::string & name, std::string & uri, std::string & chyata)
{
	size_t e = 0;
	// std::string shof_okan;
	// std::cerr << "name  chyata to9adim : " << name << std::endl;
	for (; e < name.size(); e++)
	{
		if (name[e] != uri[e])
			break ;
	}
	if (e == name.size())
	{
			chyata = uri.substr(e, uri.size());
			// if (shof_okan.size() > chyata.size())
			// 	chyata = shof_okan;
			// std::cerr << "chyata : "<< chyata << std::endl;
		return true;
	}
	return false; 
}

bool compareStringLength(const std::string& str1, const std::string& str2) {
    return str1.size() > str2.size();
}

bool sub_location_uri(std::string & uri, std::string & name)
{
	size_t e = 0;
	// std::string shof_okan;
	// std::cerr << "name  chyata to9adim : " << name << std::endl;
	for (; e < uri.size(); e++)
	{
		if (name[e] != uri[e])
			break ;
	}
	// std::cerr << "name : "  << name << std::endl;
	// std::cerr <<"e : " << e <<  " - uri.find('/', e +1) : "  << name.find("/", e +1) << " - uri.size() - 1 : " << name.size() - 1<< std::endl;
	if (e == uri.size() && name.find("/", e + 1) == name.size() - 1)
		return true;
	return false;
}

int _match_theLocation( Server &_server, Location &_location, Request &_request , Response & _response )
{
	std::vector<int> v;
	std::vector<std::string> t;

	// std::cerr << "+++++++++++++ " << _request.uri << std::endl;

	bool _find = false;
	bool check = false;
	size_t len = 0;
	// size_t len_b = 0;
	std::vector<std::string> names;
	for (size_t k =0 ; k < _server.locations.size(); k++ )
	{

		// std::cerr << "------- " << _server.locations[k].name << std::endl;
		names.push_back(_server.locations[k].name);
	}
	// std::sort(names.begin(), names.end(), compareStringLength);
	while (!_find)
	{
		size_t _ghayarha=0;
		for (size_t k =0 ; k < names.size(); k++ )
		{
			// if 
			// std::cerr << "-----------------------------k = " << k  << std::endl;
			// std::cerr << "_server.locations[k].name : " << _server.locations[k].name << " - name[k] : " << names[k] << std::endl; 
			if ((!check && names[k] == _request.uri) || (check && _server.locations[k].name == names[k]))
			{
			// std::cerr << "++++++++++++++++++++++++++k = " << k  << std::endl;
				_find = true;
				_location = _server.locations[k];
				_server.chyata = _server.locations[k].chyata;
				break ;
			}
			else if (sub_location_uri(_request.uri, names[k]))
			{
				// std::cerr <<  "$$$$$$$" << std::endl;
				_location = _server.locations[k];
				_server.chyata = _server.locations[k].chyata;
				break ;
			}
			else if (sub_location(names[k], _request.uri, _server.locations[k].chyata) && names[k].size() > len)
			{
				// std::cerr <<  "=========" << std::endl;
				_location = _server.locations[k];
				_server.chyata = _server.locations[k].chyata;
				len = names[k].size();
			}
			if (names[k].empty())
				_ghayarha++;
		}
		if (len)
			break ;
		removeLastWord(names);
		if (_ghayarha == names.size() - 1)
			return 0;
		check = true;
	}
	std::string chyata_pro_max;
	// std::cerr << "_location.name : " << _location.name << std::endl;
	if (access((_webserv_loc + "/public" + _location.root_location).c_str(), F_OK))
	{
		if (_location.redirection.path.empty())
			_response.status = 404;
		else {
			// std::cerr << "wraha zahyaaaaaaaaaaaaa" << std::endl;
			_request.uri = _location.redirection.path;
			chyata_pro_max = _server.chyata;
			_match_theLocation(_server, _location, _request, _response);
		}
	}
	if (!chyata_pro_max.empty())
		_server.chyata = chyata_pro_max;
	_request.uri = _location.root_location + _server.chyata;
	// std::cerr << "iwa hada jhdna : " << _request.uri << " - _server.chyata : "  << _server.chyata << std::endl;
	_server.chyata.clear();
	return 1;
}

bool isCharacterAllowed(std::string ss) {
    for (size_t i = 0; i < ss.size(); i++ )
    {
        if (!(std::isalnum(ss[i]) || ss[i] == '-' || ss[i] == '_' || ss[i] == '.' || ss[i] == '/'))
            return false;
    }
    return true;
}

int    _is_method_allowed( Location _location, Request &_request, Response &_response )
{
    for (size_t i=0 ; i<_location.allows_methods.size(); i++)
        if (_location.allows_methods[i] == _request.method)
            return 1;
    if (_request.method == "PUT" || _request.method == "HEAD" || _request.method == "PATCH" || _request.method == "OPTIONS" || _request.method == "TRACE")
        _response.status = 501;
    else
        _response.status = 405;
    return 0;
}

void	_validate_request( Server &_server, Location &_location, Request &_request, Response &_response )
{
    std::map<std::string, std::string>::iterator _transfer_encoding_iter = _request.headers.find("Transfer-Ecoding");
	std::map<std::string, std::string>::iterator _content_length_iter = _request.headers.find("Content-Length");

	if (!_response.status)
	{
		if (_transfer_encoding_iter != _request.headers.end() && _transfer_encoding_iter->second != "chunked")
			_response.status = 501;
		if (_transfer_encoding_iter == _request.headers.end() && _content_length_iter == _request.headers.end() && _request.method == "POST")
			_response.status = 400;
		if (!isCharacterAllowed(_request.uri))
			_response.status = 400;
		if ((_request.uri).size() > 2048)
			_response.status = 414;
		if (static_cast<int>(_request.body.size()) > (str_to_num(_server.client_max_body_size)* 1e6) )
			_response.status = 413;
		if (_is_method_allowed(_location, _request, _response))
			_request.is_method_allowed = 1;
		else
			_request.is_method_allowed = 0;
		// if (!_match_theLocation(_server, _location, _request, _response ) || (_match_theLocation(_server, _location, _request) && _location.redirection.path.size()))
		// {
		// 	if (_location.redirection.path.size())
		// 	{
		// 		_request.redirection.push_back(_location.redirection.path);
		// 		std::cerr << "REDIRECTION " << _request.redirection[0] << std::endl;
		// 		int _x = 0;
		// 		for (size_t i=0; i < _server.locations.size(); i++)
		// 		{
		// 			if (_server.locations[i].name == _request.redirection[0])
		// 			{
		// 				_location = _server.locations[i];
		// 				_response.status = _location.redirection.return_status;
		// 				_request.uri = _location.redirection.path;
		// 				_request.is_method_allowed = 1;
		// 				_x = 1;
		// 				std::cerr << "tttttttttt " << _server.locations[i].name << std::endl;
		// 				break;
		// 			}
		// 		}
		// 		if (!_x)
		// 			_response.status = 404;
		// 	}
		// 	else
		// 		_response.status = 404;
		// }
	}
	// if (_request.redirection.size())
	// {
	// 	// // verify the location
	// 	// std::cerr << "WWW" << std::endl;
	// 	// int _x = 0;
	// 	// for (size_t i=0; i< _server.locations.size(); i++)
	// 	// {
	// 	// 	if (_server.locations[i].root_location == _request.redirection[0])
	// 	// 	{
	// 	// 		_location = _server.locations[i];
	// 	// 		_response.status = _location.redirection.return_status;
	// 	// 		_x = 1;
	// 	// 	}
	// 	// }
	// 	// if (!_x)
	// 	// 	_response.status = 404;
	// }
}

void _extract_first_line( Request &_request, std::string s )
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
        _request.method = v[0];
        _request.uri = v[1];
    }
}

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
	_request.path = _webserv_loc + "/public/" + _request.uri;
	// std::cerr << "THE WHATT: " << _request.path << " THE HACK: " << _request.root << std::endl;
	// std::cerr << "```````````" << _server.name << " -- " << _location.redirection.path << std::endl;
	// if (_location.redirection.path.size())
	// 	_request.redirection.push_back(_location.redirection.path);
	// std::cerr << "```````````" << _server.name << " -- " << _request.redirection[0] << std::endl;
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
	int pos;
	size_t port = 0;
	std::string host = _request.headers["Host"];
	pos = host.find(":");
	if (pos != -1)
	{
		port = str_to_num(host.substr(pos + 1, host.size()));
		host.erase(pos, 5);
		for (size_t i=0; i<_server.servers.size(); i++)
		{
			if (_server.servers[i].listen_port == port)
			{
				_s =  _server.servers[i];
				return 1;
			}
		}
	}
    for (size_t i=0; i<_server.servers.size(); i++)
    {		
		if (_server.servers[i].name == host)
        {
			_s =  _server.servers[i];
			return 1;
		}
	}
	_s =  _server.servers[0];
	return 0;
}

int	_match_thePort( Parsing &_server, Request &_request, Server &_s)
{
	int	_pos = _request.uri.find(':');
	if (_pos >= 0)
	{
		std::string _port = _request.uri.substr(_pos+1, _request.uri.size());
		for (size_t i=0; i <_server.servers.size(); i++)
		{
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

	_extract_first_line(_request, v[0]);
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
		size_t pos = v[i].find(" ");
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
	while (encoded.get(ch))
	{
		if (ch == '%')
		{
			int hex;
			if (!(encoded >> std::hex >> hex))
				break;
			decoded << static_cast<char>(hex);
		}
		else
		  decoded << ch;
  }

  return decoded.str();
}

void check_QueryString(std::string & path, std::string & queryString)
{    
    int pos = path.find('?');
    if (pos != -1)
        queryString = path.substr(pos + 1, path.size() - pos); 

	path = path.substr(0, pos);
}

void	_request( Parsing &_server, Server &_s, Request &_request, Response &_response, std::string s )
{
	Location _location;

	_request_parser(_request, s);
	_request.uri = urlcode(_request.uri);
	check_QueryString(_request.uri, _request.queryString);
    _match_theServer(_server, _request, _s);

	// std::cerr << "_request.uri  " << _request.uri << std::endl;
	_match_theLocation(_s, _location, _request, _response );
	// std::cerr << "a " << a  << "_request.uri  " << _request.uri << std::endl;
	// if ()
	// {
	// 	;
	// }
	
	_fill_request(_s, _location, _request);
    _validate_request(_s, _location, _request, _response);
	_get_mims(_response);
	// std::cerr << "---------------" << _location.name << " -- " << _request.path << " -- " << _request.is_method_allowed << std::endl;

}
