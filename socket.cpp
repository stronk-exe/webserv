#include "webserv.hpp"

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
        t_response *_response;
		_response = _request(_server, buffer);
        

        // char *_server_message = strdup("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 18\n\ngreets from server");
		std::string s = _response->http_version+" "+_response->status+"\nContent-Type: "+_response->content_type+"\nContent-Length: "+std::to_string(_response->content_length)+"\n\ngreets from server";
        // char *v = strdup(s.c_str());
		write(coming_socket, s.c_str(), strlen(s.c_str()));
        close(coming_socket);
    }
}
