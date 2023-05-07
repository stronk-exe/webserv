#include "webserv.hpp"

void print_error(std::string s)
{
	std::cerr << s << std::endl;
    exit(1);
}

int has_index_files( t_server *_server, t_request *_request )
{
	std::ifstream _file;
	std::cerr << _server->root+_request->uri+"index.php" << std::endl;
	_file.open(_server->root+_request->uri+"index.php");
	if (_file)
		return 1;
	_file.open(_server->root+_request->uri+"index.html");
	if (_file)
		return 1;
	return 0;
}

void _get( t_request *_request, t_server *_server )
{
	int _resource_found = 0;

	std::cout << "GET" << std::endl;

	// Check if resource found
	for (size_t i=0 ; i<_server->index.size(); i++)
	{
		std::string t = _server->location[0]+_server->index[i];
		std::ifstream _file;
		_file.open(_server->root+_request->uri);
		// std::cerr << "_file: " << _file << std::endl;
		if (_file)
			_resource_found = 1;
	}
	if (!_resource_found)
		print_error("404 Not Found");
	
	// std::cerr << _server->root+_request->uri << std::endl;
	// if (_request->uri != _server->root)
	// 	print_error("404 Not Found");
	

	// Check resource type
	// std::cerr << "uri: " << _request->uri << " ~ " << _request->uri[(_request->uri).size()-1] << std::endl;
	const char* path = (_server->root +_request->uri).c_str();

    struct stat info;
    if (stat(path, &info) != 0) {
        std::cerr << "Error: cannot stat " << path << std::endl;
        exit(1);
    }

    if (S_ISDIR(info.st_mode))
		_request->type = "directory";
        // std::cout << path << " is a directory" << std::endl;
    else if (S_ISREG(info.st_mode))
		_request->type = "file";
        // std::cout << path << " is a file" << std::endl;
    else
		print_error("path is neither a file nor a directory");
        // std::cout << path << " is neither a file nor a directory" << std::endl;
    
	// if (_request->uri[_request->uri.size()-1] == '/')
	// 	_request->type = "directory";
	// else
	// 	_request->type = "file";

	// std::cerr << "type: " << _request->type << std::endl;

	if (_request->type == "directory")
	{
		if (_request->uri[_request->uri.size()-1] != '/')
		{
			_request->uri+='/';
			// std::cerr << _request->uri << std::endl;
			std::cout << "301 Moved Permanently" << std::endl;
			exit(1);
		}
		else
		{
			// Check if directory has index files
			if (has_index_files(_server, _request))
				_cgi(_server, _request);
			else
			{
				// autoindex
				if (!_request->autoindex)
					print_error("403 Forbidden");
				else
				{
					std::cout << "200 OK" << std::endl;
					exit(1);
				}
				// {
				// 	std::cerr << "403 Forbidden" << std::endl;
				// 	exit(1);
				// }
			}
			// if (_server->index.size() != 0)
			// {
			// 	// cgi
			// }
		}
	}
	else if (_request->type == "file")
	{
		//
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
