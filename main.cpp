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
    if (ac == 2)
    {
        std::fstream file(av[1]);
        std::string _file, s;

        while (std::getline(file, s))
            (_file+=s).push_back('\n');
        
        int count = _servers_count(_file);
        t_server _server[count];

        _parser(_server, _file, count);
        _socket(_server);
    }
    else
        std::cerr << "invalid number of arguments!" << std::endl;
    return 0;
}