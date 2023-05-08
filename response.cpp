#include "webserv.hpp"

t_response *_response()
{
    t_response *_response = new t_response;
    std::cout << "hadi ra response" << std::endl;
    _response->content_length = 18;
    _response->content_type = "text/plain";
    return _response;
}