#include "webserv.hpp"

int _valid_url_chars( std::string s )
{
    std::string t = "ABCDEFGHIJKLMNOPQRSTUVWXYZazcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    int x=0;
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

void _validate_request( std::map<std::string, std::string> m, t_request *_request, t_server *_server )
{
    (void)m;
    std::cerr << _request->method << std::endl;
    if (_request->method == "GET")
    {
        std::cout << "get" << std::endl;
        std::map<std::string, std::string>::iterator _transfer_encoding_it = m.find("Transfer-Ecoding"),
        _content_length_it = m.find("Content-Length");

        if (_transfer_encoding_it != m.end() && _transfer_encoding_it->second != "chunked")
        {
            std::cerr << "502 Not Emplemented" << std::endl;
            exit(1);
        }
        else if (_transfer_encoding_it == m.end() && _content_length_it == m.end())
        {
            std::cerr << "400 Bad Request" << std::endl;
            exit(1);
        }

        if (_server->host.length() > 2048)
        {
            std::cerr << "414 Request-URI Too Long" << std::endl;
            exit(1);
        }
        if (!_valid_url_chars(_server->host))
        {
            std::cerr << "400 Bad Request" << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cout << "post" << std::endl;
    }
}

void _parse_request( t_server *_server, char *s )
{
    // std::cout << "^^^^^^^^^^request^^^^^^^^^^" << std::endl;
    // // std::cout << _req << std::endl;
    // std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
    t_request _request;

    std::string _req[99];
    char *p;
    // Here, the delimiter is white space.
    p = strtok(s, "\n");
    int i=0;
    while (p != NULL) {
        // std::cout << p << std::endl;
        _req[i] = std::string(p);
        std::cerr << _req[i] << std::endl;
        p = strtok(NULL, "\n");
        i++;
    }

    std::map<std::string, std::string> m;

    std::cerr << _req[0].find("GET") << std::endl;
    if (_req[0].find("GET") >= 0)
        _request.method = "GET";
    else
        _request.method = "POST";
    for (int x=1; x < i; x++)
    {
    
        char *r = &_req[x][0];
        char *t;
        // Here, the delimiter is white space.
        t = strtok(r, ":");
        int y=0;
        std::string _key, _value;
        while (t != NULL) {
            // _req[x] = std::string(p);
            // std::cout << p << std::endl;
            if (!y%2)
                // std::cout << "key " << t << std::endl;
                _key = t;
            else
                // std::cout << "value " << t << std::endl;
                _value = t;
                
            if (_key != "\0" && _value != "\0")
            {
                m.insert(std::make_pair(_key, _value));
                // std::cout<< _key <<"    "<< _value << std::endl;
                _key = "";
                _value = "";
            }
            t = strtok(NULL, ":");
            // i++;
            y++;
        }
    }

    // std::map<std::string, std::string>::iterator iter;
    // for (iter = m.begin(); iter != m.end(); iter++)
    // {
    //   std::cout<<(*iter).first<<"   ---   "<<(*iter).second<<"\n";
    // }
    _validate_request(m, &_request, _server);
}

void _socket( t_server *_server )
{
    int _socket_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    (void)_server;

    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "socket creation failed!" << std::endl;
        exit(1);
    }

    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    int p = stoi(_server[0].port);
    address.sin_port = htons(p);

    if ((bind(_socket_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        std::cerr << "binding failed!" << std::endl;
        exit(1);
    }

    if ((listen(_socket_fd, 10)) < 0)
    {
        std::cerr << "listining failed!" << std::endl;
        exit(1);
    }

    while (1)
    {
        int coming_socket;
        std::cout << "listening on port " << p << "..." << std::endl;
        if ((coming_socket = accept(_socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            std::cerr << "acception failed!" << std::endl;
            exit(1);
        }

        char buffer[99999] = {0};
        int data = read(coming_socket, buffer, 99999);
        if (data < 0)
        {
            std::cerr << "empty data!" << std::endl;
            exit(1);
        }
        _parse_request(_server, buffer);
        

        char *_server_message = strdup("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\ngreets from server");
        write(coming_socket, _server_message, strlen(_server_message));
        close(coming_socket);
    }
}
