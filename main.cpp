#include "webserv.hpp"

int _servers_count( std::string s )
{
    std::string str= "server {";
    size_t nPos = s.find(str, 0); 
    int count = 0;
    while (nPos != std::string::npos)
    {
        count++;
        nPos = s.find(str, nPos + str.size());
    }
    return count;
}

int main(int ac, char **av)
{
    // t_request *_request;
    // t_response *_response;
    t_req *_request = new t_req;
    t_res *_response = new t_res;

    if (ac == 2)
    {
        std::fstream file(av[1]);
        std::string _file, s;

        while (std::getline(file, s))
            (_file+=s).push_back('\n');
        
        int count = _servers_count(_file);
        t_server _server[count];

        
        // 1- Config File:
        _config_parser(_server, _file, count);

        // 2- Socket connection
        _socket(_server, _request, _response);

        // 3- Request:
        // _request();

        // 4- Response:
        // _response();
    }
    else
        std::cerr << "invalid number of arguments!" << std::endl;
    return 0;
}