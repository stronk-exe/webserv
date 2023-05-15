#include <cerrno>
#include <iostream>
#include <cstdio>

int main(int ac, char *av[]) {
    FILE *pipe;
    char buffer[128];

    if (ac != 2)
        return 0;
    // Open a pipe and execute the command
    pipe = popen(av[1], "r");
    if (pipe == NULL) {
        perror("pipe");
        return 1;
    }
    std::string result;
    // Read the command output line by line
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) 
        result += buffer;

    // Close the pipe
    pclose(pipe);
    
    std::cout << result << std::endl;

    return 0;
}