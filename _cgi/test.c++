#include "../webserv.hpp" 



void _cgi(std::string scriptPath, std::string _pwd)
{
    int pipe_fd[2];
    char *av[3];
    pid_t pid;
    char buffer[100];
    std::string arg; 
    std::string args[2];

    std::istringstream ss(scriptPath);
    for (int i = 0; i < 2; i++) {
        std::getline(ss, arg, ' ');
        args[i] = _pwd + "/" + arg;
        av[i] = const_cast<char*>(args[i].c_str());
    }
    av[2] = NULL;
    if (pipe(pipe_fd) == -1) {
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    }
    pid = fork();
    if (pid == -1) {
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    }
    if (pid == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        for (int i = 0; i < 2; i++)
            std::cerr << av[i] << std::endl;
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
            std::cout <<"Script execution failed."<< std::endl;///////////////////
    }
}

int main()
{
    _cgi("cgi-bin/php-cgi scripts//script.php", "/Users/mait-jao/Project/webserv");
    return 0;
}