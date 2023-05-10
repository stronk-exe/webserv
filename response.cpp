#include "webserv.hpp"

void	_response( t_res *_response, t_req *_request )
{
    // t_response *_resp = new t_response;

    // std::cout << "hadi ra response" << std::endl;
	if (_response->status == 403)
    {
        _response->data = "<html><body><h1>403 Forbidden</h1></body></html>";
        _response->status_message = "Forbidden";
    }
    else if (_response->status == 404)
    {
        _response->data = "<html><body><h1>404 Not Found</h1></body></html>";
        _response->status_message = "Not Found";
    }
    else
    {
		_response->data = "<html><body><h1>server says: 9oraydiss</h1></body></html>";
        _response->status_message = "OK";
		// _response->status = 200;
    }
    // else
    // {
    //     _resp->data = ""
    // }
	

	// Response body
	std::cerr << "hada rah body" << std::endl;
	_request->path = "temp_root/html/index.html";
	std::cerr << "path: " << _request->path << std::endl;
	
	std::ifstream myfile;
	myfile.open(_request->path);
	std::string myline;
	_response->body = "";
	if ( myfile.is_open() )
	{
		while ( myfile ) {
			std::getline (myfile, myline);
			// std::cerr << "~~" << myline << '\n';
			_response->body += myline;
		}
	}
	else
		std::cout << "Couldn't open file\n";
	_response->body.erase(_response->body.size()-myline.size(), myline.size());

	std::cerr << "body: " << _response->body << std::endl;
	
	// Response heders
	_response->http_version = _request->http_version;
	_response->content_length = (_response->body).size();
	_response->content_type = "text/html";
	// _response->status = _response->status;
    
    // return _response;
}