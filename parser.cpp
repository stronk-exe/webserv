
#include "webserv.hpp"

void _get_ports( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;
		if ((_index = _servers[i].find("listen")) > 0)
		{
			int x=_index+6;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
				t+=_servers[i][x++];
		}
		_server[i].port = t;
	}
}

void _get_server_name( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;
		if ((_index = _servers[i].find("server_name")) > 0)
		{
			int x=_index+11;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
				t+=_servers[i][x++];
		}
		_server[i].server_name = t;
	}
}

void _get_location_block( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;
		if ((_index = _servers[i].find("location")) > 0)
		{
			int x=_index+8;
			while (_servers[i][x] != '{')
				x++;
			x++;
			while (_servers[i][x] != '}')
				t+=_servers[i][x++];
		}
		_server[i].location.push_back(t);
	}
}

std::string _trim(std::string s) {
	int atstart = 0, atend = s.size()-1;

	for(size_t i=0;i<s.size();i++)
	{
		if(s[i] != ' ')
		{
			atstart = i;
			break;
		}
	}
	for(size_t i = s.size()-1;i>=0;i--)
	{
		if(s[i] != ' ')
		{
			atend = i;
			break;
		}
	}
	std::string fistr = "";
	for(int i=atstart;i<=atend;i++)
		fistr += s[i];
	return fistr;
}

void _get_location( t_server *_server, std::string* _servers, std::string s, int c )
{
	(void)c;
	for (int i=0; i< c; i++)
	{
		std::string str= "location";
		size_t nPos = s.find(str, 0); 
		int count = 0;
		while (nPos != std::string::npos)
		{
			count++;
			nPos = s.find(str, nPos + str.size());
		}
		for (int j=0; j < count; j++)
		{
			std::string t="";
			int _index;
			if ((_index = _servers[i].find("location")) > 0)
			{
				int x=_index+8;
				while (_servers[i][x] != '{')
					t+=_servers[i][x++];
			}
			_server[i].location.push_back(_trim(t));
		}
	}
}

void _get_client_max_body_size( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;
		if ((_index = _servers[i].find("client_max_body_size")) > 0)
		{
			int x=_index+20;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
				t+=_servers[i][x++];
		}
		_server[i].client_max_body_size = t;
	}
}

void _get_index( t_server *_server, std::string s, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;
		if ((_index = s.find("index")) > 0)
		{
			int x=_index+5;
			while (s[x] == ' ')
				x++;
			while (s[x] != ';')
				t+=s[x++];
		}
		char *p;
		p = strtok(&t[0], " ");
		while (p != NULL) {
			_server->index.push_back(std::string(p));
			p = strtok(NULL, " ");
    	}
	}
}

void _get_error_page( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;
		if ((_index = _servers[i].find("error_page")) > 0)
		{
			int x=_index+11;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
				t+=_servers[i][x++];
		}
		_server[i].error_page = t;
	}
}

void _get_http_redirection( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;
		if ((_index = _servers[i].find("return")) > 0)
		{
			int x=_index+6;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
				t+=_servers[i][x++];
		}
		_server[i].http_redirection = t;

		if (t.length())
		{
			char *r = &t[0];
			char *s;
			s = strtok(r, " ");
			s = strtok(NULL, " ");
			_server[i].http_path = std::string(s);
		}
	}
}

void _get_root( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;
	// std::cerr << "whyyyy" << std::endl;
		if ((_index = _servers[i].find("root")) > 0)
		{
			int x=_index+4;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
				t+=_servers[i][x++];
		}
		_server[i].root = t;

		// if (t.length())
		// {
		// 	char *r = &t[0];
		// 	char *s;
		// 	s = strtok(r, " ");
		// 	s = strtok(NULL, " ");
		// 	_server[i].root = std::string(s);
		// }
	}
}

void _get_cgi( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;
		if ((_index = _servers[i].find("cgi_pass")) > 0)
		{
			int x=_index+8;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
				t+=_servers[i][x++];
		}
		_server[i].cgi = t;
	}
}

void _parser( t_server *_server, std::string s, int count )
{
	std::string _servers[count];
	int i=0;
	int c=0;

	while (s[i])
	{
		int index = (s.substr(i, s.length())).find("server {")+9;
		i += index;
		while (s[i] && s[i] != '}')
		{
			if (s[i] == '{')
			{
				_servers[c] += s[i++];
				i++;
				while (s[i] != '}' && s[i])
					_servers[c] += s[i++];
				_servers[c] += s[i++];
				i++;
			}
			else
				_servers[c] += s[i++];
		}
		c++;
		if (c == count)
			break;
	}


	// allocating and constracting the servers
	
	_get_ports(_server, _servers, c);
	_get_server_name(_server, _servers, c);
	_get_location(_server, _servers, s, c);
	_get_client_max_body_size(_server, _servers, c);
	_get_index(_server, s, c);
	_get_root(_server, _servers, c);
	_get_cgi(_server, _servers, c);
	_get_error_page(_server, _servers, c);
	_get_http_redirection(_server, _servers, c);

	/*for (int i=0; i<count; i++)
	{
		std::cout << "server_name: " << _server[i].server_name << std::endl;
		std::cout << "port: " << _server[i].port << std::endl;
		for (size_t j=0; j<_server[i].location.size(); j++)
			std::cout << "location: " << _server[i].location[j] << std::endl;
		std::cout << "client_max_body_size: " << _server[i].client_max_body_size << std::endl;
		std::cout << "error_page: " << _server[i].error_page << std::endl;
		std::cout << "----------------------" << std::endl;
		std::cout << "root: " << _server[i].root << std::endl;
		std::cout << "http_redirection: " << _server[i].http_redirection << std::endl;
		std::cout << "http_path: " << _server[i].http_path << std::endl;
		std::cout << std::endl;
	}*/
}