#ifndef  PARSING_HPP
#define  PARSING_HPP

# include <fstream>
# include <string>
# include <vector>
# include <stdlib.h>

namespace  http_server 
{
    struct error_page
    {
        std::vector<int>  error_status;
        std::string       path;
    };

    struct locations
    {
        bool            autoindex;
        std::string  root_location, name;
        std::vector<std::string>  index;
        std::vector<std::string>  allows_methods;
        std::vector<std::string>  cgi_pass;
    };
    

    struct server
    {
        std::string  name, root_location;
        std::vector<std::string>  index;
        std::vector<locations>   location;
        size_t  listen_port;
        error_page  error;
    };

    struct parsing
    {
        std::string     file;
        std::vector<std::string> data;
        server  server;
    };
    
}

#endif
