#include "parsing.hpp"


void split(std::vector<std::string> &data, char del1, char del2, std::string str) 
{
    std::string tmp;

            std::cout << str << std::endl;

    for (size_t i = 0; i < str.size() ; i++)
    {
        if ((str[i] == del1 || str[i] == del2))
        {
            if (!tmp.empty())
                data.push_back(tmp);
            tmp = "";
            continue;
        }
        else
            tmp += str[i];
    }
    if (!tmp.empty())
        data.push_back(tmp);
}

int main(int ac, char *av[])
{
    std::string  str;
    http_server::parsing parss;

    if (ac == 2) 
    {
        parss.file = av[1];
        std::ifstream file(parss.file);
        while (std::getline(file, str))
            split(parss.data, ' ', '\t', str);
            std::cout << "****************" << std::endl;
        std::vector<std::string>::iterator it;
        for (it = parss.data.begin(); it != parss.data.end(); it++)
            std::cout << *it << std::endl;
        file.close();
    }
    return 0;
}
