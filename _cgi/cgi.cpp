/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 13:05:56 by mait-jao          #+#    #+#             */
/*   Updated: 2023/05/19 17:58:22 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp" 

// bool get_first_arg(char str[], std::string &s)
// {
//     char *token = strtok(str, " ");
//     if (token[0] == '#' && token[1] == '!') {
//         s = token;
//         return true;
//     }
//     return false;
// }

// void get_command(std::string &str, std::string _file)
// {
//     std::ifstream file(_file);
//     while (std::getline(file, str))
//     {
//         if (get_first_arg(const_cast<char *>(str.c_str()), str))
//             break;
//     }
//     str.erase(str.begin(), str.begin() + 2);
// }

// void path_file(std::string root, std::string file, char **str)
// {
//     std::string tmp = root + file;/////dosnt work 
//     *str = const_cast<char *>(tmp.c_str());
// }

std::string int_to_str(int num)
{
    std::ostringstream str1;
    str1 << num;
    return str1.str();
}

void update_env_for_cgi( Request *_request , std::string _path_info, Server _server )
{
    if (setenv("PATH_INFO", _path_info.c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("REQUEST_METHOD", _request->method.c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("CONTENT_TYPE", _request->headers["CONTENT_TYPE"].c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("CONTENT_LENGTH", _request->headers["CONTENT_LENGTH"].c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("HTTP_USER_AGENT", _request->headers["User-Agent"].c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("DOCUMENT_ROOT", _request->root.c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("SCRIPT_NAME", _request->path.c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("REMOTE_ADDR", "127.0.0.1", 1) != 0) {/////hardcod
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }    
    if (setenv("REMOTE_HOST", _request->headers["Host"].c_str(), 1) != 0) {/////hardcod
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("SERVER_PORT", int_to_str(_server.listen_port).c_str(), 1) != 0) {/////hardcod
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }    
    if (setenv("SERVER_NAME", _server.name.c_str(), 1) != 0) {/////hardcod
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }    
    if (setenv("HTTP_COOKIE", _request->headers["Cookie"].c_str(), 1) != 0) {/////hardcod
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
}

void     printHeaders(std::map<std::string, std::string> headers)
{
    std::map<std::string, std::string>::iterator i;
        // std::cerr << i->first << " " << i->second << std::endl;
        std::cerr << "*********************************************" << std::endl;
    for ( i = headers.begin(); i != headers.end(); i++)
    {
        std::cerr << i->first << " -> " << i->second << std::endl;
    }
        std::cerr << "*********************************************" << std::endl;
}

bool check_extension(std::string extension, std::string path)
{
    std::string::reverse_iterator it_ext, it_path;
    
    extension.insert(0, ".");
    for (it_ext = extension.rbegin(), it_path = path.rbegin(); it_ext != extension.rend() ; it_ext++, it_path++)
    {
        if (*it_ext != *it_path)
            return false;
    }
    return true;
}

bool check_path_extension(std::vector<CGI>	cgi_pass, std::string path, std::string &scriptPath )
{
    std::vector<CGI>::iterator it;
    
    for (it = cgi_pass.begin(); it != cgi_pass.end() ; it++)
    {
        if (check_extension((*it).extension, path))
        {
            scriptPath = (*it).path + " " + path;
            return true;
        }
    }
    return false;
}

std::string get_content_type(char *buffer)
{
    std::string str;
    
    char* token = std::strtok(buffer, " ");
    token = std::strtok(NULL, " ");
    
    return (str = token);
}

void	_cgi( Request *_request, Response *_response , Server _server )
{
    // printHeaders(_request->headers);
	std::string result, scriptPath;
    // std::cerr << "(*_request->cgi.begin()) =" << (*_request->cgi.begin())  << "| _request->path="<< _request->path <<  std::endl;
    if (!check_path_extension(_request->cgi , _request->path, scriptPath)) {
        std::cerr << "extansion" << std::endl;
        _response->body = "";
        return ;
    }
    //  = (*(_request->cgi.begin() + 1)) +//_request->path;
    // std::cerr << "scriptPath : [" << scriptPath << "]" << std::endl;
    update_env_for_cgi(_request, scriptPath, _server);
    FILE* pipe = popen(scriptPath.c_str(), "r");
    if (!pipe) {
        std::cerr << strerror(errno) << std::endl;
        return ;
    }
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        // std::cerr << "------------" << buffer <<"--------------" << strncmp(buffer, "Content-type", 13) << std::endl;
        if (!strncmp(buffer, "Content-type", 12))
            _response->content_type = get_content_type(buffer);
        else
            result += buffer;
    }
    pclose(pipe);
	_response->body = result;
    
	std::cerr << "Content-type: |" << _response->content_type << "|" << std::endl;
	std::cerr << "execution output: " << _response->body << std::endl;
    _response->status = 200;
}

// void _cgi(Request *_request, Response *_response)
// {
//     int pipe_fd[2];
//     pid_t pid;
//     char buffer[100];

//     if (pipe(pipe_fd) == -1) {
//         std::cerr << strerror(errno) << std::endl;
//         exit(12);
//     }
// 	char *av[3];
// 	path_file(_request->root, *(_request->cgi.begin() + 1), &av[0]);
// 	path_file(_request->root, _request->path, &av[1]);
// 	av[2] = NULL;
//     pid = fork();
//     if (pid == -1) {
//         std::cerr << strerror(errno) << std::endl;
//         exit(12);
//     }
//     if (pid == 0) {
//         close(pipe_fd[0]);
//         dup2(pipe_fd[1], STDOUT_FILENO);
//         close(pipe_fd[1]);
//         execve(av[0], av, NULL);
//         std::cerr << strerror(errno) << std::endl;
//         exit(12);
//     } else {
//         std::string result;
//         close(pipe_fd[1]);
//         int nbytes;
//         while ((nbytes = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
//             result.append(buffer, nbytes);
//         close(pipe_fd[0]);
//         int status;
//         waitpid(pid, &status, 0);
//         if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
//             _response->body = result;
//         else
//             std::cout <<"Script execution failed."<< std::endl;///////////////////
//     }
// }
