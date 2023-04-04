
#include "webserv.hpp"

void _get_ports( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;// = _servers[i].find("listen");
		if ((_index = _servers[i].find("listen")) > 0)
		{
			int x=_index+6;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
			{
				t+=_servers[i][x++];
				// x++;
			}
		}
		_server[i].port = t;
	}
}

void _get_server_name( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;// = _servers[i].find("server_name");
		if ((_index = _servers[i].find("server_name")) > 0)
		{
			int x=_index+11;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
			{
				t+=_servers[i][x++];
				// y++;
			}
		}
	// std::cerr << "Poo"<< c << std::endl;
		_server[i].server_name = t;
	}
}

void _get_location_block( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;// = _servers[i].find("location");
		if ((_index = _servers[i].find("location")) > 0)
		{
			int x=_index+8;
			// std::cerr << "indexxxxxx: " << location_block_index << "\n***\n"<< _servers[i] << "---------" << std::endl;
			// std::cout << "1--------" << std::endl;
			while (_servers[i][x] != '{')
			{
				// std::cout << _servers[i][x];
				x++;
			}
			// std::cout << "2--------" << std::endl;
			x++;
			while (_servers[i][x] != '}')
			{
				// std::cout << _servers[i][x];
				t+=_servers[i][x++];
				// x++;
			}
		}
		// std::cout << "3--------" << std::endl;
		_server[i].location.push_back(t);
		// std::cerr << "malna" << std::endl;
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
				// std::cerr << "indexxxxxx: " << location_block_index << "\n***\n"<< _servers[i] << "---------" << std::endl;
				// std::cout << "1--------" << std::endl;
				while (_servers[i][x] != '{')
				{
					// std::cout << _servers[i][x];
					t+=_servers[i][x++];
					// x++;
				}
				// std::cout << "2--------" << std::endl;
				// x++;
				// while (_servers[i][x] != '}')
				// {
				// 	// std::cout << _servers[i][x];
				// 	// x++;
				// }
			}
			// std::cout << "3--------" << std::endl;
			// t.trim_to_left();
			_server[i].location.push_back(_trim(t));
			// std::cerr << "malna" << std::endl;
		}
	}
}

void _get_client_max_body_size( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;// = _servers[i].find("client_max_body_size");
		if ((_index = _servers[i].find("client_max_body_size")) > 0)
		{
			int x=_index+20;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
			{
				t+=_servers[i][x++];
				// y++;
			}
		}
		_server[i].client_max_body_size = t;
	}
}

void _get_error_page( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;// = _servers[i].find("error_page");
		if ((_index = _servers[i].find("error_page")) > 0)
		{
			int x=_index+11;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
			{
				t+=_servers[i][x++];
				// y++;
			}
		}
		_server[i].error_page = t;
	}
}

void _get_http_redirection( t_server *_server, std::string* _servers, int c )
{
	for (int i=0; i< c; i++)
	{
		std::string t="";
		int _index;// = _servers[i].find("return");
		if ((_index = _servers[i].find("return")) > 0)
		{
			// std::cerr << _index << std::endl;
			int x=_index+6;
			while (_servers[i][x] == ' ')
				x++;
			while (_servers[i][x] != ';')
			{
				t+=_servers[i][x++];
				// y++;
			}
		}
		_server[i].http_redirection = t;

		if (t.length())
		{
			char *r = &t[0];
			char *s;
			s = strtok(r, " ");
			// std::cout << s << std::endl;
			s = strtok(NULL, " ");
			_server[i].http_path = std::string(s);
			// std::cout << s << std::endl;
			// (*s)++;
		}
	}
}

void _parser( t_server *_server, std::string s, int count )
{
	// int count = 0;
	// std::string str= "server {";
    // size_t nPos = s.find(str, 0); 
	// while (nPos != std::string::npos)
	// {
	// 	count++;
	// 	nPos = s.find(str, nPos + str.size());
	// }

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
				{
					_servers[c] += s[i++];
					// i++;
				}
				_servers[c] += s[i++];
				i++;
			}
			else
			{
				_servers[c] += s[i++];
				// i++;
			}
		}
		c++;
		if (c == count)
			break;
	}


	// allocating and constracting the servers
	

	_get_ports(_server, _servers, c);
	_get_server_name(_server, _servers, c);
	// _get_location_block(_server, _servers, c);
	_get_location(_server, _servers, s, c);
	_get_client_max_body_size(_server, _servers, c);
	_get_error_page(_server, _servers, c);

	_get_http_redirection(_server, _servers, c);
	// _get_http_path(_server, _servers, c);
	
	// for (int i=0; i< c; i++)
	// {
	// 	// std::string port="";
	// 	// int port_index = _servers[i].find("listen");
	// 	// int x=port_index+6;
	// 	// while (_servers[i][x] == ' ')
	// 	// 	x++;
	// 	// while (_servers[i][x] != ';')
	// 	// {
	// 	// 	port+=_servers[i][x];
	// 	// 	x++;
	// 	// }


	// 	// std::string host="";
	// 	// int host_index = _servers[i].find("server_name");
	// 	// int y=host_index+11;
	// 	// while (_servers[i][y] == ' ')
	// 	// 	y++;
	// 	// while (_servers[i][y] != ';')
	// 	// {
	// 	// 	host+=_servers[i][y];
	// 	// 	y++;
	// 	// }
		
	// 	// std::cout << i << " - " << _servers[i]  << std::endl;
	// 	// _server[i].port = port;
	// 	_server[i].host = host;
	// 	// std::cerr << "port " << _server[i].port << std::endl;
	// 	// std::cerr << "host " << _server[i].host << std::endl;
	// }
	for (int i=0; i<count; i++)
	{
		std::cout << "server_name: " << _server[i].server_name << std::endl;
		std::cout << "port: " << _server[i].port << std::endl;
		for (size_t j=0; j<_server[i].location.size(); j++)
			std::cout << "location: " << _server[i].location[j] << std::endl;
		std::cout << "client_max_body_size: " << _server[i].client_max_body_size << std::endl;
		std::cout << "error_page: " << _server[i].error_page << std::endl;
		std::cout << "----------------------" << std::endl;
		std::cout << "http_redirection: " << _server[i].http_redirection << std::endl;
		std::cout << "http_path: " << _server[i].http_path << std::endl;
		std::cout << std::endl;
	}

	// while (s[i])
	// {
	// 	// if (s[i] == '{')
	// 	// {
	// 	// 	while (s[i] && s[i] != '}')
	// 	// 	{
	// 	// 		std::cout << s[i];
	// 	// 		i++;
	// 	// 	}
	// 	// }
	// 	std::cout << s[i];
	// 	i++;
	// }
	// and the parsing begins!!
	// if (strcmp())
}