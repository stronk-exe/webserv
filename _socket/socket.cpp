/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:14:33 by ael-asri          #+#    #+#             */
/*   Updated: 2023/06/11 20:39:05 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

std::string	num_to_str(ssize_t num)
{
	std::ostringstream str1;
    str1 << num;
    return str1.str();
}

std::string	generate_response_str(Client & client )
{
	std::string response = "HTTP/1.1 "+ std::to_string(client._response.status)+" "+client._response.status_message+\
					"\nContent-Type: "+client._response.content_type+\
					"\nContent-Length: "+std::to_string(client._response.content_length);
	
	if (!client.cookies.empty())
		response += "\n" + client.cookies;
	if (!client._response.location.empty())
		response += "\nLocation: "+client._response.location;
					
	response += "\n\n"+client._response.body;

	return response;
}

std::vector<int>	_get_ports( Parsing &_server )
{
    std::vector<int>    _ports;

	for (size_t i=0; i < _server.servers.size(); i++)
		_ports.push_back(_server.servers[i].listen_port);
	return _ports;
}

int	find_client(std::vector <Client> _cls, int _id)
{
	for (size_t i =0; i < _cls.size(); i++)
	{
		if (_cls[i]._id == _id)
			return _id;
	}
	return -1;
}

bool	isFileDescriptorAvailable(int fd) {
    int flags = fcntl(fd, F_GETFD);
    return (flags != -1);
}

ssize_t get_content_length(std::string &req)
{
	int pos0, pos1;
	std::string s;
    

    pos0 = req.find("Content-Length:");
    if (pos0 == -1)
        pos0 = req.find("Content-length:");
	if (pos0 == -1)
		return -1;
    pos1 = req.find("\n", pos0);
	s = req.substr(pos0 + 15 , pos1 - pos0 - 16);
	// std::cerr << "|" << s  << "|" << std::endl;
	return (str_to_num(s));
}

void	_socket( Parsing &_server )
{
    int					_socket_fd;
    struct sockaddr_in	address;
    int					addrlen;
    int					default_port;
	std::vector<int>	_socket_fds;
	std::vector<Client> Clients;
	fd_set				_sockets, _current_sockets, _readfds, _writefds;
	
	addrlen = sizeof(address);
	default_port = _get_ports(_server)[0];

	// Initializing the sockets
	FD_ZERO(&_sockets);
	FD_ZERO(&_current_sockets);
	FD_ZERO(&_readfds);
	FD_ZERO(&_writefds);
	for (size_t i=0; i < _server.servers.size(); i++)
	{
		// Creating a socket for each server
		if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			print_error("socket creation failed!");
		
		// Binding the sockets of each server
		memset((char *)&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl(INADDR_ANY);
		address.sin_port = htons(_server.servers[i].listen_port);

		int on = 1;
		if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
			print_error("port in use!");
		fcntl(_socket_fd, F_SETFL, O_NONBLOCK);

		if ((bind(_socket_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
			print_error("binding failed!");
		
		// Start listining..
		if ((listen(_socket_fd, SOMAXCONN)) < 0)
			print_error("listining failed!");
		
		
		FD_SET(_socket_fd, &_readfds);
		_socket_fds.push_back(_socket_fd);

	}
	int fd_size = _socket_fds[_socket_fds.size() - 1];
	// int					_reading_lock=0, _writing_lock=0;
	std::string _test_buffer;

	std::vector<int> accepted_shit;
    while (1)
    {
        std::cout << "listening ..." << std::endl;
		
		_sockets = _readfds;
		_current_sockets = _writefds;
		if (select(fd_size + 1, &_sockets, &_current_sockets, NULL, NULL) < 0)
			print_error("error in select");
		int coming_socket;
    
		for (int x=0; x <= fd_size; x++)
		{


			if (FD_ISSET(x, &_sockets) || FD_ISSET(x, &_current_sockets))
			{
				for (size_t e=0; e<Clients.size(); e++)
				{
					if (FD_ISSET(Clients[e]._id, &_sockets) && FD_ISSET(Clients[e]._id, &_current_sockets))
					{
						close(Clients[e]._id);
						FD_CLR(Clients[e]._id, &_readfds);
						FD_CLR(Clients[e]._id, &_writefds);
                        Clients[e]._done_writing = 0;
						std::vector<Client>::iterator it = Clients.begin();
						std::advance(it, e);
						Clients.erase(it);
					}
				}


				if (std::find(_socket_fds.begin(), _socket_fds.end(), x) != _socket_fds.end())
				{
					if ((coming_socket = accept(x, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
						print_error("acception failed!");
					else
						std::cerr << "\033[1;92mACCEPTINGGGGGGGGGGGGGG\e[0m _id : " << coming_socket << std::endl;
					fcntl(coming_socket, F_SETFL, O_NONBLOCK);
					FD_SET(coming_socket, &_readfds);
					if (coming_socket > fd_size)
						fd_size = coming_socket;
					Clients.push_back(Client(coming_socket));
					break ;
				}
				
				for (size_t e=0; e<Clients.size(); e++)
				{
					if (x == Clients[e]._id && (std::find(_socket_fds.begin(), _socket_fds.end(), Clients[e]._id) == _socket_fds.end()) &&  Clients[e]._read_status)
					{
						std::cerr << "\033[1;95mREADINGGGGGGGGGGGGGG \e[0m _id : "<< Clients[e]._id << std::endl;
						int check = 0;
						char				buffer[999999] = {0};

						Clients[e].data = read(Clients[e]._id, buffer, 999999);
						if (Clients[e].data > 0)
						{
							for (int i=0; i<Clients[e].data; i++)
								Clients[e].buffer += buffer[i];
							if (Clients[e].prsing_req.empty())
								check = 1;///////// intial 

							Clients[e].prsing_req = Clients[e].buffer;
							if (check == 1)
								Clients[e].post_legnth = get_content_length(Clients[e].prsing_req);
							// std::cerr << " Clients[e].post_legnth :: " << Clients[e].post_legnth << std::endl;

						}
						if (Clients[e].post_legnth >= 0 && static_cast<ssize_t>(Clients[e].prsing_req.size()) > Clients[e].post_legnth)
						{
							std::cout << "----------" << std::endl;
								Clients[e]._done_reading = 1;
								Clients[e]._read_status = 0;
								FD_SET(Clients[e]._id, &_writefds);
						}

						else if (Clients[e].post_legnth == -1 && (Clients[e].data <= 0 || Clients[e].data < 999999))
						{
								
								Clients[e]._done_reading = 1;
								Clients[e]._read_status = 0;
								FD_SET(Clients[e]._id, &_writefds);
						}
					}
					// Request parsing
					else if (x == Clients[e]._id && Clients[e]._done_reading && !Clients[e]._read_status && !Clients[e]._write_status)
					{
						std::cerr << "\033[38;5;214mPARSINGGGGGGGGGGGGGG \e[0m_id : "<< Clients[e]._id  << std::endl;
						Server _s;
						_request(_server, _s, Clients[e]._request, Clients[e]._response, Clients[e].prsing_req);
	
						// Checking the method
						if (Clients[e]._request.is_method_allowed && Clients[e]._response.status != 400)
						{
							if (Clients[e]._request.method == "GET")
								_get(Clients[e], _s);
							else if (Clients[e]._request.method == "POST")
								_post(Clients[e], _s);
							else if (Clients[e]._request.method == "DELETE")
								_delete(Clients[e]);
						}
						else if (Clients[e]._response.status != 400)
							Clients[e]._response.status = 405;
			
						_response(Clients[e]);
						Clients[e].s = generate_response_str(Clients[e]);
						Clients[e]._write_status = 1;
						FD_SET(Clients[e]._id, &_writefds);
					}
					else if (x == Clients[e]._id && std::find(_socket_fds.begin(), _socket_fds.end(), Clients[e]._id) == _socket_fds.end() && Clients[e]._write_status/* && !Clients[e]._done_writing*/)
					{
						std::cerr << "\033[1;94mWRITINGGGGGGGGGGGGGG \e[0m_id : "<< Clients[e]._id << std::endl;
						if (waitpid(Clients[e]._cgi_pid, &Clients[e].status, WNOHANG) > 0)
						{
							if (remove(Clients[e].file.c_str()))
               					 strerror(errno);
							Clients[e]._kill_pid = true;
							if (WIFSIGNALED(Clients[e].status) && (WTERMSIG(Clients[e].status) == SIGALRM))
							{
								Clients[e]._response.status = 508;
								_response(Clients[e]);
							}
							else
                            {
								parent_process( Clients[e].body, Clients[e].pipe_fd);
								get_body(Clients[e]);
							}
							Clients[e].s = generate_response_str(Clients[e]);
						}
						if (Clients[e]._kill_pid)
						{
							if (isFileDescriptorAvailable(Clients[e]._id))
								Clients[e].return_write = write(Clients[e]._id, &Clients[e].s[Clients[e]._wr], Clients[e].s.size()-Clients[e]._wr);
							if (Clients[e].return_write > 0)
								Clients[e]._wr += Clients[e].return_write;
							if (Clients[e]._done_writing)
							{
								std::cerr << "\033[1;91mDROPIGGGGGGGGGG CLIENT \e[0m_id : "<< Clients[e]._id << std::endl;

								close(Clients[e]._id);
								FD_CLR(Clients[e]._id, &_readfds);
								FD_CLR(Clients[e]._id, &_writefds);
								std::vector<Client>::iterator it = Clients.begin();
								std::advance(it, e);
								Clients.erase(it);
								break;
							}
							else
							{
								_get_res_body(Clients[e]);
								Clients[e].s = generate_response_str(Clients[e]);
							}
						}

					}
				}
			}
		}
	}
}
