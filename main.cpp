/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:14:48 by ael-asri          #+#    #+#             */
/*   Updated: 2023/05/18 12:59:05 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int ac, char **av)
{
    Request *_request = new Request;
    Response *_response = new Response;

    std::string  str;
    Parsing _server;

    if (ac == 2)
    {
        _server.file = av[1];
        std::ifstream file(_server.file);
        if (!file) {
            std::cerr << "Failed to open the file." << std::endl;
            return 1;
        }
        while (std::getline(file, str))
            split_conf(_server.data, str);

        // 1- Config File:
        parss_info(_server);
        std::cout << (_server.servers.begin())->name << std::endl;
        std::cout << (_server.servers.begin())->listen_port << std::endl;
        // print_data(_server);

        // 2- Socket connection
        _socket(_server, _request, _response);
    }
    else
        std::cerr << "invalid number of arguments!" << std::endl;
    return 0;
}
