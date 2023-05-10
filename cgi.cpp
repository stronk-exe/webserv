#include "webserv.hpp"

void _cgi( t_server *_server, t_res *_response )
{
	// (void)_request;
	if (_server->cgi.size())
	{
		std::cout << "cgi UwU" << std::endl;
		_response->status = 200;
	}
	else
	{
		// std::cout << "200 OK" << std::endl;
		// _response(_request, 200);
		exit(1);
	}
}