

#include "../webserv.hpp"

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
        {
            if (str[i] == '#')
                break ;
            else
                tmp += str[i];
        }
    }
    if (!tmp.empty())
        data.push_back(tmp);
}

void error(std::string err)
{
    std::cerr << "\033[1;91m" << err << std::endl;
    exit(1337);
}

int str_to_num(std::string str)
{
    char *_p;
    size_t _int;
    
    _int = std::strtol(str.c_str(), &_p, 10);
    if (*_p != '\0')
        error("number error");
    return _int;
}

void info_err_status(std::vector<error_page> &errors, std::vector<std::string>::iterator &it)
{
    error_page err;

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

std::vector<std::string> info_(std::vector<std::string>::iterator &it)
{
    std::vector<std::string>  vec;
    for (; * (it + 1) != ";" ; it++)
        vec.push_back(*it);
    vec.push_back(*it);
    return vec;
}

void info_autoindex(Location &loc, std::string &str) 
{
    if (str == "on")
        loc.autoindex = true;
    else if (str == "off")
        loc.autoindex = false;
    else
        error("autoindex error");
}

CGI info_cgi( std::vector<std::string>::iterator &it )
{
    CGI _struct;
    std::vector<std::string> vec = info_(it);
    if (vec.size() != 2)
        error("cgi_pass path error");
    _struct.extension = *vec.begin();
    _struct.path = *(vec.begin() + 1);
    return _struct;    
}

void info_location(std::vector<Location> &locations, std::vector<std::string>::iterator &it)
{
    Location loc;

    if (!(*it != "{" && *(it + 1) == "{"))
        error("location bracket error");
    loc.name = *it;
    for (it += 2; *it != "}"; it += 2)
    {
        if (*it == "root" && *(it + 1) != ";" && *(it + 2) == ";")
            loc.root_location = *(++it);
        else if (*it == "index")
            loc.index = info_(++it); 
        else if (*it == "upload" && *(it + 1) != ";" && *(it + 2) == ";")
            loc.uploadDir = *(++it);
        else if (*it == "allow_methods")
            loc.allows_methods = info_(++it);
        else if (*it == "cgi_pass")
            loc.cgi_pass.push_back(info_cgi(++it)); 
        else if (*it == "autoindex" && *(it + 1) != ";" && *(it + 2) == ";") 
            info_autoindex(loc, *(++it));
        else if (*it == "return" && *(it + 1) != ";" && *(it + 2) != ";" && *(it + 3) == ";"){
            loc.redirection.return_status = str_to_num(*(++it));
            loc.redirection.path = *(++it);
        }
        else 
            error("not understood");
    }
    if (*it == "}")
        locations.push_back(loc);
    else 
        error("server bracket");
    it--;
}

void info_listen(Server &serv, std::vector<std::string>::iterator &it)
{

    std::string word;
    std::vector<std::string> vec;
    std::istringstream iss(*it);
    
    while (std::getline(iss, word, ':'))
        vec.push_back(word);
    if (vec.size() > 2)
        error("listen_port error");
    if (vec.size() == 2) {
        if (serv.name.empty())
            serv.name = *vec.begin();
        serv.listen_port = str_to_num(*(vec.begin() + 1));
    }
    else if (vec.size() == 1){
        serv.listen_port = str_to_num(*vec.begin());
    }
}


void info_nameServ(Server &serv, std::string &data)
{
    int pos = data.find(":");
    if (pos != -1)
    {
        serv.name = data.substr(0, pos);
        serv.listen_port = str_to_num(data.substr(pos + 1, data.size()));
    }
    else
        serv.name = data;
}

void parss_info(Parsing &parss)
{
    Server serv;
    std::vector<std::string>::iterator it;
    
    for (it = parss.data.begin(); it != parss.data.end(); it++)
    {
        
        if (*it == "server" && *(++it) == "{")
        {
            for (it++; *it != "}"; it += 2)
            {
                if (*it == "server_name" && *(it + 1) != ";" && *(it + 2) == ";")
                    info_nameServ(serv, *(++it));
                else if (*it == "root" && *(it + 1) != ";" && *(it + 2) == ";")
                    serv.root_location = *(++it);
                else if (*it == "client_max_body_size" && *(it + 1) != ";" && *(it + 2) == ";")
                    serv.client_max_body_size = *(++it);   
                else if (*it == "listen" && *(it + 1) != ";" && *(it + 2) == ";")
                    info_listen(serv, ++it);
                else if (*it == "error_page")
                    info_err_status(serv.errors, ++it);
                else if (*it == "index")
                    info_(++it);   
                else if (*it == "location")
                    info_location(serv.locations, ++it);
                else if (*it == "return" && *(it + 1) != ";" && *(it + 2) != ";" && *(it + 3) == ";"){
                    serv.redirection.return_status = str_to_num(*(++it));
                    serv.redirection.path = *(++it);
                }
                else if (*it != ";")
                    error("not understood");
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


void print_str(std::vector<std::string> &vec, const char *str)
{
    std::vector<std::string>::iterator it;
    std::cout << str << " ";
    for ( it = vec.begin(); it != vec.end(); it++)
        std::cout  << (*it) <<" ";
    std::cout << "\n";
}

 void print_int(std::vector<int> &vec)
{
    std::vector<int>::iterator it;
    for ( it = vec.begin(); it != vec.end(); it++)
        std::cout << (*it) << " ";
}

void print_err(std::vector<error_page> &vec)
{
    std::vector<error_page>::iterator it;
    int i = 0;
    for ( it = vec.begin(); it != vec.end(); it++, i++) {
        std::cout <<"\n" <<  i << ":  " << "errors.path" << (*it).path << std::endl;
        print_int((*it).error_status);
    }
}

void print_cgi(std::vector<CGI> &vec)
{
    std::vector<CGI>::iterator it;
    int i = 0;
    for ( it = vec.begin(); it != vec.end(); it++, i++) {
        std::cout <<"\n" <<  i << ":  " << "CGI.path=" << (*it).path << std::endl;
        std::cout <<"\n" <<  i << ":  " << "CGI.extension=" << (*it).extension << std::endl;
    }
}

void print_loc(std::vector<Location> &vec)
{
    std::vector<Location>::iterator it;
    int i = 0;
    for ( it = vec.begin(); it != vec.end(); it++, i++) {
        std::cout << "\n---------------location----------------" << std::endl;

        std::cout << i << ":  "<< "location.autoindex "  <<  (*it).autoindex << std::endl;
        std::cout << i << ":  "<< "location.name "  <<  (*it).name << std::endl;
        std::cout << i << ":  "<< "location.root_location " <<  (*it).root_location << std::endl;
        std::cout << i << ":  "<< "location.upload " <<  (*it).uploadDir << std::endl;
        print_str((*it).index, "index ");
        print_str((*it).allows_methods, "allows_methods ");
        print_cgi((*it).cgi_pass);

        std::cout << "\n-------------------------------" << std::endl;
    }
}

void print_data(Parsing &parss)
{
    std::vector<Server>::iterator it;
    int i = 0;
    for ( it = parss.servers.begin(); it != parss.servers.end(); it++, i++)
    {
        std::cout << "---------------server----------------" << std::endl;
        std::cout << i << ":  "<< "server.name "  <<  (*it).name << std::endl;
        std::cout << i << ":  "<< "server.root_location " <<  (*it).root_location << std::endl;
        std::cout << i << ":  "<< "server.listen_port " <<  (*it).listen_port << std::endl;
        print_str((*it).index, "index ");
        print_err((*it).errors);
        print_loc((*it).locations);
        // std::cout << "server.name " << i << " " <<  (it).name << std::endl;
        std::cout << "\n-------------------------------" << std::endl;
        // std::cout << "server.name " << i << " " <<  (*it).name << std::endl;
    }

}

// int main(int ac, char *av[])
// {
//     std::string  str;
//     Parsing parss;

//     if (ac == 2) 
//     {
//         parss.file = av[1];
//         std::ifstream file(parss.file);
//         while (std::getline(file, str))
//             split_conf(parss.data, str);
//         parss_info(parss);
//         print_data(parss);
//         file.close();
//     }
//     return 0;
// }