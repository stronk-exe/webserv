/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-asri <ael-asri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:15:15 by ael-asri          #+#    #+#             */
/*   Updated: 2023/05/11 11:15:16 by ael-asri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

void _cgi( Request *_request, Response *_response )
{
	// Temporary
	std::cerr << "pathhh: " << _request->path << std::endl;
	// _request->path += "index.html";
	// (void)_request;
	// if (_server.name.size())
	// {
		std::cout << "cgi UwU" << std::endl;
		_response->status = 200;
	// }
	// else
	// {
	// 	// std::cout << "200 OK" << std::endl;
	// 	// _response(_request, 200);
	// 	exit(1);
	// }
}