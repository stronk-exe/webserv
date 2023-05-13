#ifndef  PARSING_HPP
#define  PARSING_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>
# include <cstdlib>

namespace  http_server 
{
    struct error_page
    {
        std::vector<int>  error_status;
        std::string       path;
    };

    struct location
    {
        bool            autoindex;
        std::string  root_location, name;
        std::vector<std::string>  indexs;
        std::vector<std::string>  allows_methods;
        std::vector<std::string>  cgi_pass;
    };
    

    struct server
    {
        std::string  name, root_location;
        std::vector<std::string>  indexs, allow_methods;
        std::vector<location>   locations;
        std::vector<error_page>  errors;
        size_t  listen_port;
    };

    struct parsing
    {
        std::string     file;
        std::vector<std::string> data;
        std::vector<server>  servers;
    };
    
}

void error(std::string err);
size_t str_to_num(std::string str);
void parss_info(http_server::parsing &parss);
void info_autoindex(http_server::location &loc, std::string &str);
void info_(std::vector<std::string>  &vec, std::vector<std::string>::iterator &it);
void split_conf(std::vector<std::string> &data, std::string str);
void info_err_status(std::vector<http_server::error_page> &errors, std::vector<std::string>::iterator &it);
void info_location(std::vector<http_server::location> &locations, std::vector<std::string>::iterator &it);

#endif
