#include "webserv.hpp"

t_response *_response( t_request *_request, std::string _msg )
{
    t_response *_resp = new t_response;

    std::cout << "hadi ra response" << std::endl;
    _resp->http_version = _request->http_version;
    _resp->content_length = 18;
    _resp->content_type = "text/plain";
    _resp->status = _msg;
    
    return _resp;
}