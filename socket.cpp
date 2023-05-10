#include "webserv.hpp"

void _socket( t_server *_server, t_req *request, t_res *response )
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
        // t_response *_response;
		_request(_server, request, buffer);

        // checking the method
        if (request->method == "GET")
        {
            // std::cout << "location: " << _server->location[0] << std::endl;
            _get(response, request, _server);
        }
        else if (request->method == "POST")
        {
            _post();
        }
        else if (request->method == "DELETE")
        {
            _delete();
        }
        else
        {
            std::cerr << "405 Method Not Allowed" << std::endl;
            exit(1);
        }

        // Response
        _response(response, request);
        std::cout << "wssa3 ya kho response jat:" << std::endl;
        std::cout << "response content_length " << response->content_length << std::endl;
        std::cout << "response content_type " << response->content_type << std::endl;
        std::cout << "response status " << response->status << std::endl;
        // std::cout << "response content_type " << response->content_type << std::endl;
        

        // char *_server_message = strdup("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 18\n\ngreets from server");
        // _response->data = "<html><body><h1>server says: 9oraydiss</h1></body></html>";
        std::cerr << "was here" << std::endl;
        // std::cerr << "HTTP/1.1 "+ std::to_string(_response->status)+" "+_response->status_message << std::endl;
		std::string s = "HTTP/1.1 "+ std::to_string(response->status)+" "+response->status_message+"\nContent-Type: "+response->content_type+"\nContent-Length: "+std::to_string(response->content_length)+"\n\n"+response->body;
        // char *v = strdup(s.c_str());
		write(coming_socket, s.c_str(), strlen(s.c_str()));
        close(coming_socket);
    }
}
