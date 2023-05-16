/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 13:05:56 by mait-jao          #+#    #+#             */
/*   Updated: 2023/05/16 14:35:26 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp" 

bool get_first_arg(char str[], std::string &s)
{
    char *token = strtok(str, " ");
    if (token[0] == '#' && token[1] == '!') {
        s = token;
        return true;
    }
    return false;
}

void get_command(std::string &str, std::string _file)
{
    std::ifstream file(_file);
    while (std::getline(file, str))
    {
        if (get_first_arg(const_cast<char *>(str.c_str()), str))
            break;
    }
    str.erase(str.begin(), str.begin() + 2);
}

void cgi_pass(std::string s)
{
    int pipe_fd[2];
    pid_t pid;
    char buffer[100];

    if (pipe(pipe_fd) == -1) {
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    }
    std::string str;
    get_command(str, s);
	char *av[3];
	av[1] = const_cast<char *>(s.c_str());
	av[2] = NULL;
	av[0] = &str[0];
    pid = fork();
    if (pid == -1) {
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    }
    if (pid == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        execve(av[0], av, NULL);
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    } else {
        std::string result;
        close(pipe_fd[1]);
        int nbytes;
        while ((nbytes = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
            result.append(buffer, nbytes);
        close(pipe_fd[0]);
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            std::cout << result << std::endl;
        else
            std::cout <<"Script execution failed."<< std::endl;
    }
}