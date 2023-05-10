#include "webserv.hpp"

int _location_not_found( t_server *_server, t_req *_request )
{
    for (size_t i=0; i < _server->location.size(); i++)
    {
        std::cerr << _server->location[i] << " - " << _request->uri << std::endl;
        if (_server->location[i] == _request->uri)
            return 1;
    }
    return 0;
}

int _valid_url_chars( std::string s )
{
    std::string t = "ABCDEFGHIJKLMNOPQRSTUVWXYZazcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    size_t x=0;
    for (int i=0; s[i]; i++)
    {
        for (int j=0; t[j]; j++)
            if (s[i] == t[j])
                x++;
    }
    if (x != s.length())
        return 0;
    return 1;
}

void	_validate_request( std::map<std::string, std::string> m, t_req *_request )
{
    // (void)_server;
	// t_response *_response=NULL;
    std::map<std::string, std::string>::iterator _transfer_encoding_it = m.find("Transfer-Ecoding");
    std::cerr << _request->method << std::endl;
    // if (_request->method == "GET")
    // {
        // std::cout << "GET" << std::endl;

        if (_transfer_encoding_it != m.end() && _transfer_encoding_it->second != "chunked")
        {
            std::cerr << "502 Not Emplemented" << std::endl;
            exit(1);
        }
        

        if (_request->uri.length() > 2048)
        {
            std::cerr << "414 Request-URI Too Long" << std::endl;
            exit(1);
        }
        if (!_valid_url_chars(_request->uri))
        {
            std::cerr << "400 Bad Request" << std::endl;
            exit(1);
        }

        // if (!_location_not_found(_server, _request))
        // {
        //     std::cerr << "404 Not Found" << std::endl;
        //     exit(1);
        // }

        // // checking the method
        // // t_resource _resource;
        // if (_request->method == "GET")
        // {
        //     // std::cout << "location: " << _server->location[0] << std::endl;
        //     _response = _get(_request, _server);
        // }
        // else if (_request->method == "POST")
        // {
        //     _post();
        // }
        // else if (_request->method == "DELETE")
        // {
        //     _delete();
        // }
        // else
        // {
        //     std::cerr << "405 Method Not Allowed" << std::endl;
        //     exit(1);
        // }
    // }
    // else
    // {
    //     std::cout << "POST" << std::endl;
    //     if (_transfer_encoding_it == m.end() && _content_length_it == m.end())
    //     {
    //         std::cerr << "400 Bad Request" << std::endl;
    //         exit(1);
    //     }
    // }
    // return _response;
}

void _extract_first_line( t_req *_request, char *s )
{
    std::vector<std::string> v;
    char *_line = strtok(s, " ");
    while (_line != NULL)
    {
        v.push_back(_line);
        _line = strtok(NULL, " ");
    }

    if (v.size() == 3)
    {
        _request->method = v[0];
        _request->uri = v[1];
        _request->http_version = v[2];
    }
    else
    {
        std::cerr << "missing method or uri or http-version here!" << std::endl;
        exit(1);
    }
}

void	_request( t_server *_server, t_req *_request, char *s )
{
    // t_request _request;
	// t_request *_request = new t_request;

    std::vector<std::string> _req;
    char *p;
    // Here, the delimiter is white space.
    p = strtok(s, "\n");
    while (p != NULL) {
        _req.push_back(std::string(p));
        p = strtok(NULL, "\n");
    }

    std::map<std::string, std::string> m;



    // extract the method, the uri and the http-version
    _extract_first_line(_request, &_req[0][0]);
    for (size_t x=1; x < _req.size(); x++)
    {
    
        char *r = &_req[x][0];
        char *t;
        // Here, the delimiter is white space.
        t = strtok(r, ":");
        int y=0;
        std::string _key, _value;
        while (t != NULL)
        {
            if (!y%2)
                _key = t;
            else
                _value = t;
                
            if (_key != "\0" && _value != "\0")
            {
                m.insert(std::make_pair(_key, _value));
                _key = "";
                _value = "";
            }
            t = strtok(NULL, ":");
            y++;
        }
    }
	_request->index = _server->index;

    // std::map<std::string, std::string>::iterator iter;
    // for (iter = m.begin(); iter != m.end(); iter++)
    // {
    //   std::cout<<(*iter).first<<"   ---   "<<(*iter).second<<"\n";
    // }
	// t_response *_response;
    _validate_request(m, _request);
    // std::cout << "wssa3 ya kho response jat:" << std::endl;
    // std::cout << "response content_length " << _response->content_length << std::endl;
    // std::cout << "response content_type " << _response->content_type << std::endl;
    // return _validate_request(m, _request, _response);
}
