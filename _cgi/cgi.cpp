/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 13:05:56 by mait-jao          #+#    #+#             */
/*   Updated: 2023/06/11 20:26:39 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp" 

std::string int_to_str(int num)
{
    std::ostringstream str1;
    str1 << num;
    return str1.str();
}

void update_env_for_cgi( Request &_request , std::string _path_info, Server _server )
{
    _request.env[0] = strdup(("PATH_INFO=" + _path_info).c_str());
    _request.env[1] = strdup(("REQUEST_METHOD=" + _request.method).c_str());
    _request.env[2] = strdup(("CONTENT_TYPE=" + _request.headers["Content-Type"]).c_str());
    _request.env[3] = strdup(("CONTENT_LENGTH=" + _request.headers["Content-Length"]).c_str());
    _request.env[4] = strdup(("SCRIPT_FILENAME=" + _path_info).c_str());
    _request.env[5] = strdup(("SERVER_PORT=" + int_to_str(_server.listen_port)).c_str());
    _request.env[6] = strdup(("SERVER_NAME=" + _server.name).c_str());
    _request.env[7] = strdup(("HTTP_COOKIE=" + _request.headers["Cookie"]).c_str());
    _request.env[8] = strdup(("QUERY_STRING=" + _request.queryString).c_str());
    _request.env[9] = strdup("REDIRECT_STATUS=200");
    _request.env[10] = NULL;
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

bool check_path_extension(std::vector<CGI>	&cgi_pass, std::string &path, std::string &scriptPath )
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
    str = token;
    return (str);
}


void parent_process(Client & _client)
{
    char buffer[4056];
    int nbytes;

    close(_client.pipe_fd[1]);
    if ((nbytes = read(_client.pipe_fd[0], buffer, sizeof(buffer))) > 0)
        _client.body.append(buffer, nbytes);
    if (nbytes >= 0)
    {
        close(_client.pipe_fd[0]);
        get_body(_client);
    }
    else
    {
        close(_client.pipe_fd[0]);
        _client._done_writing = 1;
    }

}


int createFile(const char* filename, std::string data) {
    
    int fd = open(filename, O_WRONLY | O_CREAT, 0644);

    if (!(fd != -1 && write(fd, data.c_str(), data.size()) != -1))
        return -1;
    close(fd);
    fd = open(filename, O_RDONLY, 0644);

    return fd;
}

void exec_file_cgi(std::string &scriptPath, Client & client)
{
    char *av[3];
    int fd;
    
    std::string result, arg, args[2];
    
    std::istringstream ss(scriptPath);
    for (int i = 0; i < 2; i++) {
        std::getline(ss, arg, ' ');
        args[i] = arg;
        av[i] = const_cast<char*>(args[i].c_str());
    }
    av[2] = NULL;
    client._kill_pid = false;
    client.file = _webserv_loc + "/_cgi/" + generateRandomString(7, ".h");
    fd = createFile(client.file.c_str() , client._request.body);
    if (fd == -1)
    {
        client._response.status = 500;
        return ;
    }
    if (!(pipe(client.pipe_fd) > -1 && (client._cgi_pid = fork()) > -1))
        exit(12);
    if (client._cgi_pid == 0)
    {
        dup2(fd, STDIN_FILENO);
        close(client.pipe_fd[0]);
        dup2(client.pipe_fd[1], STDOUT_FILENO);
        close(client.pipe_fd[1]);
        alarm(25);
        execve(av[0], av, client._request.env);
        exit(1337);
    }
    else
        waitpid(client._cgi_pid, &client.status, WNOHANG);
}

std::vector<std::string> extractSetCookieSubstrings(const std::string& text) {
    std::vector<std::string> substrings;

    std::string::size_type pos = text.find("Set-Cookie");
    while (pos != std::string::npos)
    {
        std::string::size_type endPos = text.find('\n', pos);
        if (endPos != std::string::npos)
        {
            std::string substring = text.substr(pos, endPos - pos);
            substrings.push_back(substring);
            pos = text.find("Set-Cookie", endPos);
        }
        else
            break;
    }

    return substrings;
}

void set_cookies(std::string & Cookies, std::string & result)
{
    std::vector<std::string> arry = extractSetCookieSubstrings(result);
    for (size_t i = 0; i < arry.size(); i++)
        Cookies += arry[i] + "\n";
    if (!Cookies.empty())
        Cookies.erase(Cookies.end() - 1);
}

void get_body( Client & client)
{
    std::string arg;
    int pos0, pos1;
    
    set_cookies(client.cookies, client.body);
    pos0 = client.body.find("Content-Type:");
    if (pos0 == -1)
        pos0 = client.body.find("Content-type:");
    pos1 = client.body.find("\n", pos0);
    if (pos0 != -1)
    {
        client._response.content_type = client.body.substr(pos0 + 13 , pos1 - pos0 - 13);
        client.body.erase(0, pos1 + 1);
    }
    else
        client._response.content_type = "text/html";
    
    client._response.body = client.body;
    client._response.content_length = client.body.size();
    client._response.status = 200;
}

std::string generateRandomString(int length, std::string ss) {

    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result = ss;
    int charsetLength = charset.length();

    std::srand(std::time(0)); // Seed the random number generator

    for (int i = 2; i < length; ++i) {
        int randomIndex = std::rand() % charsetLength;
        result += charset[randomIndex];
    }

    return result;
}

void	_cgi( Client & client , Server &_server )
{
	std::string result, scriptPath, arg, _pwd;
    
    update_env_for_cgi(client._request, (client._request.path), _server);
    if (!check_path_extension(client._request.cgi , client._request.path, scriptPath))
    {
        client._response.body = "";
        return ;
    }
    exec_file_cgi(scriptPath, client);
}
