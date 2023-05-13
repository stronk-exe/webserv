#include "parsing.hpp"

void split_conf(std::vector<std::string> &data, std::string str) 
{
    std::string tmp;

    for (size_t i = 0; i < str.size() ; i++)
    {
        if ((str[i] == ' ' || str[i] == '\t'))
        {
            if (!tmp.empty())
                data.push_back(tmp);
            tmp = "";
            continue;
        }
        else if (str[i] == ';')
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

void error(std::string err)
{
    std::cerr << "\033[1;91m" << err << std::endl;
    exit(1337);
}

size_t str_to_num(std::string str)
{
    char *_p;
    size_t _int;
    
    _int = std::strtol(str.c_str(), &_p, 10);
    if (*_p != '\0')
        error("number error");
    return _int;
}

void info_err_status(std::vector<http_server::error_page> &errors, std::vector<std::string>::iterator &it)
{
    http_server::error_page err;

    for (; * (it + 1) != ";" ; it++)
        err.error_status.push_back(str_to_num(*it));
    if (!(*it).empty())
    {
        err.path = *it;
        errors.push_back(err);
    }
    else
        error("location of the error page, not set");
}

void info_(std::vector<std::string>  &vec, std::vector<std::string>::iterator &it)
{
    for (; * (it + 1) != ";" ; it++)
        vec.push_back(*it);
}

void info_autoindex(http_server::location &loc, std::string &str) 
{
    if (str == "on")
        loc.autoindex = true;
    else if (str == "off")
        loc.autoindex = true;
    else
        error("autoindex error");
}

void info_location(std::vector<http_server::location> &locations, std::vector<std::string>::iterator &it)
{
    http_server::location loc;

    if (!(*it != "{" && *(it + 1) == "{"))
        error("location bracket error");
    loc.name = *it;
    for (it += 2; *it != "}"; it++)
    {
        if (*it == "root" && *(it + 1) != ";" && *(it + 2) == ";")
            loc.root_location = *(++it);
        else if (*it == "index")
            info_(loc.indexs, ++it); 
        else if (*it == "allow_methods")
            info_(loc.allows_methods, ++it); 
        else if (*it == "cgi_pass")
            info_(loc.cgi_pass, ++it); 
        else if (*it == "autoindex" && *(it + 1) != ";" && *(it + 2) == ";") 
            info_autoindex(loc, *(++it));
        else 
            error("not understood");
        it++;
    }
    if (*it == "}")
        locations.push_back(loc);
    else 
        error("server bracket");
    it--;
}

void parss_info(http_server::parsing &parss)
{
    http_server::server serv;
    std::vector<std::string>::iterator it;
    
    for (it = parss.data.begin(); it != parss.data.end(); it++)
    {
        if (*it == "server" && *(++it) == "{")
        {
            for (it++; *it != "}"; it++)
            {
                if (*it == "server_name" && *(it + 1) != ";" && *(it + 2) == ";")
                    serv.name = *(++it);
                else if (*it == "root" && *(it + 1) != ";" && *(it + 2) == ";")
                    serv.root_location = *(++it);
                else if (*it == "listen" && *(it + 1) != ";" && *(it + 2) == ";")
                    serv.listen_port = str_to_num(*(++it));
                else if (*it == "error_page")
                    info_err_status(serv.errors, ++it);
                else if (*it == "index")
                    info_(serv.indexs, ++it);   
                else if (*it == "location")
                    info_location(serv.locations, ++it);
                else
                    error("not understood");
                it++;
            }
            if (*it == "}")
                parss.servers.push_back(serv);
            else 
                error("server bracket");
        }
        else
            error("not understood");
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
            split_conf(parss.data, str);
        parss_info(parss);
        file.close();
    }
    return 0;
}
