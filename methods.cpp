#include "webserv.hpp"

void _get( t_request *_request, t_server *_server )
{
	int _resource_found = 0;

	std::cout << "GET" << std::endl;
	// Check if resource found
	for (size_t i=0 ; i<_server->index.size(); i++)
	{
		std::string t = _server->location[0]+_server->index[i];
		std::ifstream _file;
		_file.open("./root"+t);
		if (_file)
			_resource_found = 1;
		// std::cout << "index: " << t << std::endl;
	}
	if (!_resource_found)
	{
		std::cerr << "404 Not Found" << std::endl;
        exit(1);
	}

	// Check resource type
	std::cerr << "uri: " << _request->uri << " ~ " << _request->uri[(_request->uri).size()-1] << std::endl;
	if (_request->uri[_request->uri.size()-1] == '/')
	{
		_request->type = "directory";
	}
	else
	{
		_request->type = "file";
	}

	std::cerr << "type: " << _request->type << std::endl;

	if (_request->type == "directory")
	{
		if (_request->uri[_request->uri.size()-1] != '/')
		{
			std::cout << "301 Moved Permanently" << std::endl;
			exit(1);
		}
		else
		{
			if (_server->index.size() != 0)
			{
				// cgi
				_cgi();
			}
			else
			{
				// autoindex
				if (!_request->autoindex)
				{
					std::cerr << "403 Forbidden" << std::endl;
					exit(1);
				}
				else
				{
					std::cout << "200 OK" << std::endl;
					exit(1);
				}
			}
		}
	}
}

void _post()
{
	std::cout << "POST" << std::endl;
}

void _delete()
{
	std::cout << "DELETE" << std::endl;
}
