#include "webserv.hpp"

int main()
{
    int _socket_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "socket creation failed!" << std::endl;
        exit(1);
    }

    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8000);

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
        std::cout << "listening on port " << 8000 << "..." << std::endl;
        if ((coming_socket = accept(_socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            std::cerr << "acception failed!" << std::endl;
            exit(1);
        }

        char buffer[9999] = {0};
        int data = read(coming_socket, buffer, 1024);
        if (data < 0)
        {
            std::cerr << "empty data!" << std::endl;
            exit(1);
        }

        char *_server_message = strdup("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\ngreets from server!");
        write(coming_socket, _server_message, strlen(_server_message));
        close(coming_socket);
    }
}
