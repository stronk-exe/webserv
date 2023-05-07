#include "webserv.hpp"

void _cgi( t_server *_server, t_request *_request )
{
	(void)_request;
	if (_server->cgi.size())
	{
		std::cout << "cgi UwU" << std::endl;
	}
	else
	{
		std::cout << "200 OK" << std::endl;
		exit(1);
	}
}