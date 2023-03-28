#include "webserv.hpp"

int main(int ac, char **av)
{
    if (ac == 2)
    {
        std::fstream file(av[1]);
        std::string _file, s;

        while (std::getline(file, s))
            (_file+=s).push_back('\n');
        _parser(_file);
    }
    else
        std::cerr << "invalid number of arguments!" << std::endl;
    return 0;
}