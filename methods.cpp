#include "webserv.hpp"

void _get( t_resource *_resource )
{
	std::cout << "GET" << std::endl;
	// Check resource type
	if (_resource->type == "file")
	{

	}
	else if (_resource->type == "directory")
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
