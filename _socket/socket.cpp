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
	// std::cerr << "\033[1;91mDROPIGGGGGGGGGG CLIENT \e[0m_id : "<< _client._id << std::endl;

	close(_client._id);
	FD_CLR(_client._id, &_socket._readfds);
	FD_CLR(_client._id, &_socket._writefds);
	std::vector<Client>::iterator it = _socket.Clients.begin();
	std::advance(it, e);
	_socket.Clients.erase(it);
}

bool _Accepting ( Socket & _socket )
{
	for (size_t e=0; e < _socket.Clients.size(); e++)
	{
		if (FD_ISSET(_socket.Clients[e]._id, &_socket._sockets) && FD_ISSET(_socket.Clients[e]._id, &_socket._current_sockets))
			_Droping (_socket, _socket.Clients[e], e );
	}

	if (std::find(_socket._socket_fds.begin(), _socket._socket_fds.end(), _socket.x) != _socket._socket_fds.end())
	{
		if ((_socket.coming_socket = accept(_socket.x, (struct sockaddr *)&_socket.address, (socklen_t*)&_socket.addrlen)) < 0)
			print_error("acception failed!");
		else
			// std::cerr << "\033[1;92mACCEPTINGGGGGGGGGGGGGG\e[0m _id : " << _socket.coming_socket << std::endl;
		fcntl(_socket.coming_socket, F_SETFL, O_NONBLOCK);
		FD_SET(_socket.coming_socket, &_socket._readfds);
		if (_socket.coming_socket > _socket.fd_size)
			_socket.fd_size = _socket.coming_socket;
		_socket.Clients.push_back(Client(_socket.coming_socket));
		return true ;
	}
	return false ;
}

void _Reading ( Socket & _socket , Client & _client )
{
	int 	_check = 0;
	char	buffer[999999] = {0};

	// std::cerr << "\033[1;95mREADINGGGGGGGGGGGGGG \e[0m _id : "<< _client._id << std::endl;
	_client.data = read(_client._id, buffer, 999999);
	if (_client.data > 0)
	{
		for (int i=0; i<_client.data; i++)
			_client.buffer += buffer[i];
		if (_client.prsing_req.empty())
			_check = 1;///////// intial 

		_client.prsing_req = _client.buffer;
		if (_check == 1)
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
}


void _Parsing ( Socket & _socket , Client & _client )
{
	Server _s;

	// std::cerr << "\033[38;5;214mPARSINGGGGGGGGGGGGGG \e[0m_id : "<< _client._id  << std::endl;
	_request(_socket._server, _s, _client._request, _client._response, _client.prsing_req);
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
		_client.return_write = write(_client._id, &_client.s[_client._wr], _client.s.size()- _client._wr);
	if (_client.return_write > 0)
		_client._wr += _client.return_write;
	if (_client.return_write == -1 || _client._done_writing)
	{
		_Droping (_socket, _client , e );
		return true ;
	}
	else
	{
		_get_res_body(_client);
		_client.s = generate_response_str(_client);
	}
	return false ;
}

void check_cgi_end(Client & _client )
{
	if (waitpid(_client._cgi_pid, &_client.status, WNOHANG) > 0)
	{
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
			parent_process( _client.body, _client.pipe_fd);
			get_body(_client);
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
	FD_ZERO(&_socket._sockets);
	FD_ZERO(&_socket._current_sockets);
	FD_ZERO(&_socket._readfds);
	FD_ZERO(&_socket._writefds);
	for (size_t i=0; i < _socket._server.servers.size(); i++)
	{
		// Creating a socket for each server
		if ((_socket._socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			print_error("socket creation failed!");
		
		// Binding the sockets of each server
		memset((char *)&_socket.address, 0, sizeof(_socket.address));
		_socket.address.sin_family = AF_INET;
		_socket.address.sin_addr.s_addr = htonl(INADDR_ANY);
		_socket.address.sin_port = htons(_socket._server.servers[i].listen_port);
		int on = 1;
		if (setsockopt(_socket._socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
			print_error("port in use!");
		fcntl(_socket._socket_fd, F_SETFL, O_NONBLOCK);

		if ((bind(_socket._socket_fd, (struct sockaddr *)&_socket.address, sizeof(_socket.address))) < 0)
			print_error("binding failed!");
		
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
    while (1)
    {
        std::cout << "listening ..." << std::endl;
		
		_socket._sockets = _socket._readfds;
		_socket._current_sockets = _socket._writefds;
		if (select(_socket.fd_size + 1, &_socket._sockets, &_socket._current_sockets, NULL, NULL) < 0)
			print_error("error in select");
		_socket.coming_socket = 0;
    
		for (int x=0; x <= _socket.fd_size; x++)
		{
			_socket.x = x;
			if (FD_ISSET(x, &_socket._sockets) || FD_ISSET(x, &_socket._current_sockets))
			{
				if (_Accepting ( _socket ))
					break ;
				
				for (size_t e=0; e < _socket.Clients.size(); e++)
				{
					if (x == _socket.Clients[e]._id && (std::find(_socket._socket_fds.begin(), _socket._socket_fds.end(), _socket.Clients[e]._id) == _socket._socket_fds.end()) &&  _socket.Clients[e]._read_status)
						_Reading ( _socket , _socket.Clients[e] );
					else if (x == _socket.Clients[e]._id && _socket.Clients[e]._done_reading && !_socket.Clients[e]._read_status && !_socket.Clients[e]._write_status)
						_Parsing ( _socket , _socket.Clients[e] );
					else if (x == _socket.Clients[e]._id && std::find(_socket._socket_fds.begin(), _socket._socket_fds.end(), _socket.Clients[e]._id) == _socket._socket_fds.end() && _socket.Clients[e]._write_status)
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
