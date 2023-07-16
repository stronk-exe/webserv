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
		return 0;
    pos1 = req.find("\n", pos0);
	s = req.substr(pos0 + 15 , pos1 - pos0 - 16);
	return (str_to_num(s));
}

void _Droping ( Socket & _socket , Client & _client , size_t e )
{
	std::cerr << "\033[1;91mDROPIGGGGGGGGGG CLIENT \e[0m_id : "<< _client._id << std::endl;

	FD_CLR(_client._id, &_socket._readfds);
	FD_CLR(_client._id, &_socket._writefds);
	close(_client._id);
	std::vector<Client>::iterator it = _socket.Clients.begin();
	std::advance(it, e);
	_socket.Clients.erase(it);
}

bool _Accepting ( Socket & _socket )
{

	if (std::find(_socket._socket_fds.begin(), _socket._socket_fds.end(), _socket.x) != _socket._socket_fds.end())
	{
		if ((_socket.coming_socket = accept(_socket.x, (struct sockaddr *)&_socket.address, (socklen_t*)&_socket.addrlen)) < 0)
			print_error("acception failed!");
		std::cerr << "\033[1;92mACCEPTINGGGGGGGGGGGGGG\e[0m _id : " << _socket.coming_socket << std::endl;
		fcntl(_socket.coming_socket, F_SETFL, O_NONBLOCK);

		FD_SET(_socket.coming_socket, &_socket._readfds);
		if (_socket.coming_socket > _socket.fd_size)
			_socket.fd_size = _socket.coming_socket;
		_socket.Clients.push_back(Client(_socket.coming_socket));
		return true ;
	}
	return false ;
}

bool _Reading ( Socket & _socket , Client & _client )
{
	char	buffer[_BUFFER_SIZE_] = {0};

	_client.data = read(_client._id, buffer, _BUFFER_SIZE_);
	if (_client.data == -1)
		return false;
	if (_client.data > 0)
	{
		std::string temp="";
		for ( int i=0; i<_client.data; i++)
			temp += buffer[i];
		_client.prsing_req += temp;
		_client.post_legnth = get_content_length(_client.prsing_req);
	}
	if (_client.post_legnth && static_cast<ssize_t>(_client.prsing_req.size()) > _client.post_legnth)
	{
		_client._done_reading = 1;
		_client._read_status = 0;
		FD_SET(_client._id, &_socket._writefds);
	}

	else if (_client.post_legnth == 0 && (_client.data <= 0 || _client.data < 999999))
	{
		_client._done_reading = 1;
		_client._read_status = 0;
		FD_SET(_client._id, &_socket._writefds);
	}
	return true;
}


void _Parsing ( Socket & _socket , Client & _client )
{
	Server _s;

	_request(_socket._server, _s, _client );

	// Checking the method
	if (_client._request.is_method_allowed && _client._response.status != 400)
	{
		if (_client._request.method == "GET")
			_get(_client, _s);
		else if (_client._request.method == "POST")
			_post(_client, _s);
		else if (_client._request.method == "DELETE")
			_delete(_client);
	}
	
	_response(_client);
	_client.s = generate_response_str(_client);
	_client._write_status = 1;
	FD_SET(_client._id, &_socket._writefds);
}


bool _Writing ( Socket & _socket , Client & _client , size_t e )
{
	// std::cerr << "\033[1;94mWRITINGGGGGGGGGGGGGG \e[0m_id : "<< _client._id << std::endl;
	if (isFileDescriptorAvailable(_client._id) && _client.s.size()-_client._wr)
		_client.return_write = write(_client._id, &_client.s[_client._wr], _client.s.size() - _client._wr);
	if (_client.return_write > 0)
		_client._wr += _client.return_write;
	// std::cerr << "lseek(_client.fd_file, 0, SEEK_END) : " << lseek(_client.fd_file, 0, SEEK_END)<< std::endl;
    // std::cerr  << " - _client._response.content_length : " <<_client._response.content_length << std::endl;
    // std::cerr << "_client._wr : "<< _client._wr << std::endl;
	if (_client.return_write == -1 ||  _client._done_writing)
	{
		_Droping (_socket, _client , e );
		return true ;
	}
	else
	{
		_get_res_body(_client, _client._request.path);
		_client.s = generate_response_str(_client);
		if (_client._done_writing)
		{
			_Droping (_socket, _client , e );
			return true ;
		}

	}
	return false ;
}

void check_cgi_end(Client & _client )
{
	if (waitpid(_client._cgi_pid, &_client.status, WNOHANG) > 0)
	{
		std::cerr <<"1111111####################################" << std::endl;
		if (remove(_client.file.c_str()))
    		 strerror(errno);
		_client._kill_pid = true;
		if (WIFSIGNALED(_client.status) && (WTERMSIG(_client.status) == SIGALRM))
		{
			_client._response.status = 508;
			_response(_client);
		}
		else
        {
		std::cerr <<"22222222####################################" << std::endl;
			parent_process( _client);
			
		}
		_client.s = generate_response_str(_client);
	}
}

void init_socket( Socket &_socket , Parsing &_server )
{
	_socket._server = _server;
	_socket.addrlen = sizeof(_socket.address);
	_socket.default_port = _get_ports(_socket._server)[0];

	// Initializing the sockets
	FD_ZERO(&_socket._read_sockets);
	FD_ZERO(&_socket._write_sockets);
	FD_ZERO(&_socket._readfds);
	FD_ZERO(&_socket._writefds);
	struct addrinfo hints;
	for (size_t i=0; i < _socket._server.servers.size(); i++)
	{
    	memset(&hints, 0, sizeof(hints));
    	hints.ai_family = AF_INET;
    	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_flags = AI_PASSIVE;
		if (getaddrinfo((_socket._server.servers[i].ip_add).c_str(), num_to_str(_socket._server.servers[i].listen_port).c_str(), &hints, &_socket.bind_address))
			print_error("getaddrinfo failed!");

		if ((_socket._socket_fd = socket(_socket.bind_address->ai_family, _socket.bind_address->ai_socktype, _socket.bind_address->ai_protocol)) < 0)
			print_error("socket creation failed!");
		int on = 1;
		fcntl(_socket._socket_fd, F_SETFL, O_NONBLOCK);
		if (setsockopt(_socket._socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
			print_error("port in use!");

		if ((bind(_socket._socket_fd, _socket.bind_address->ai_addr, _socket.bind_address->ai_addrlen)) < 0)
			print_error("binding failed!");
		
		freeaddrinfo(_socket.bind_address);

		// Start listining..
		if ((listen(_socket._socket_fd, SOMAXCONN)) < 0)
			print_error("listining failed!");
		FD_SET(_socket._socket_fd, &_socket._readfds);
		_socket._socket_fds.push_back(_socket._socket_fd);
	}
	_socket.fd_size = _socket._socket_fds[_socket._socket_fds.size() - 1];
}

void	_socket( Parsing &_server )
{
	Socket _socket;
	
	init_socket(_socket, _server);
	// struct timeval _timeout;
    while (1)
    {
        std::cout << "listening ..." << std::endl;
		
		// _timeout.tv_sec = 0;
		// _timeout.tv_usec = 500000;
		_socket._read_sockets = _socket._readfds;
		_socket._write_sockets = _socket._writefds;
		if (select(_socket.fd_size + 1, &_socket._read_sockets, &_socket._write_sockets, NULL, NULL) < 0) {
			std::cerr	<< strerror(errno) << std::endl;
			print_error("error in select");
		}
		_socket.coming_socket = 0;
    
		for (int x=0 ; x <= _socket.fd_size; x++)
		{
			if (FD_ISSET(x, &_socket._read_sockets) || FD_ISSET(x, &_socket._write_sockets))
			{
				_socket.x = x;
				if (_Accepting ( _socket ))
					break ;
				
				for (size_t e=0; e < _socket.Clients.size(); e++)
				{
					if (x == _socket.Clients[e]._id && (FD_ISSET(x, &_socket._read_sockets) || FD_ISSET(x, &_socket._write_sockets)))
					{
						if (FD_ISSET(_socket.Clients[e]._id, &_socket._read_sockets) && FD_ISSET(_socket.Clients[e]._id, &_socket._write_sockets))
						{
							_Droping( _socket , _socket.Clients[e], e );
							break ;
						}
						if (_socket.Clients[e]._read_status)
						{
							if (!_Reading ( _socket , _socket.Clients[e]))
							{
								_Droping( _socket , _socket.Clients[e], e );
								break;
							}
						}
						// Request parsing
						if (_socket.Clients[e]._done_reading && !_socket.Clients[e]._read_status && !_socket.Clients[e]._write_status)
							_Parsing ( _socket , _socket.Clients[e] );
						if (_socket.Clients[e]._write_status)
						{
							check_cgi_end( _socket.Clients[e] );
							if (_socket.Clients[e]._kill_pid)
							{
								if (_Writing ( _socket , _socket.Clients[e] , e))
									break ;
							}
						}
					}
				}
			}
		}
	}
}
