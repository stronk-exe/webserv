#include "parsing.hpp"




void split(std::vector<std::string> &data, char del1, char del2, char del3, std::string str) 
{
    std::string tmp;

    for (size_t i = 0; i < str.size() ; i++)
    {
        if ((str[i] == del1 || str[i] == del2))
        {
            if (!tmp.empty())
                data.push_back(tmp);
            tmp = "";
            continue;
        }
        else if (str[i] == del3)
        {
            if (!tmp.empty())
                data.push_back(tmp);
            data.push_back(tmp = str[i]);
            tmp = "";
        }
        else
            tmp += str[i];
    }
    if (!tmp.empty())
        data.push_back(tmp);
}

// void info_server(http_server::parsing &parss, std::vector<std::string>::iterator &it)
// {
//     std::vector<std::string>::iterator it;
//     for (it = parss.data.begin(); it != parss.data.end(); it++)
//     {
//         if (*it == "server")
//             info_server(parss, it);
//     };
// }

size_t str_to_num(char *str)
{
    char *p;
    size_t _int;
    
    _int = std::strtol(str.c_str(), &_p, 10);
    if (*_p != '\0')
        error("number error");
    return _int;
}

void info_err_status(http_server::error_page &error, std::vector<std::string>::iterator &it)
{
    for (; it != parss.data.end(); it++);
}

void parss(http_server::parsing &parss)
{
    http_server::server serv;
    std::vector<std::string>::iterator it;
    
    for (it = parss.data.begin(); it != parss.data.end(); it++)
    {
        if (*it == "server" && *(++it) == "{")
        {
            for (it++; it != parss.data.end(); it++)
            {
                if (*it == "server_name" && *(it + 1) != ";" && *(it + 2) != ";")
                    serv.name = (++it);
                else if (*it == "listen" && *(it + 1) != ";" && *(it + 2) != ";")
                    serv.listen_port = str_to_num(*(++it));
                else if (*it == "error_page")
                    info_err_status(serv.error);
                it++
            }
        }
        else
            error();
    }
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
            split(parss.data, ' ', '\t', ';', str);
        // std::vector<std::string>::iterator it;
        // for (it = parss.data.begin(); it != parss.data.end(); it++)
        //     std::cout << *it << std::endl;
        get_info(parss);
        file.close();
    }
    return 0;
}
