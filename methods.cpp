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
		_file.open(t);
		if (_file)
			_resource_found = 1;
		// if (t)
		std::cout << "index: " << t << std::endl;
	}
	if (!_resource_found)
	{
		std::cerr << "404 Not Found" << std::endl;
        exit(1);
	}
	// Check resource type
	if (_request->uri[_request->uri.size()-1] == '\\')
	{
		_request->type = "directory";
	}
	else
	{
		_request->type = "file";
	}

	if (_request->type == "directory")
	{

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
