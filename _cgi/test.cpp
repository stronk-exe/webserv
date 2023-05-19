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

#include <cstdio>
#include <cstring>

int main() {
    // Delete a file
    const char* filename = "path/to/file.txt";
    if (std::remove(filename) != 0) {
        std::perror("Error deleting the file");
        return 1;
    }
    else {
        std::puts("File deleted successfully");
    }

    // Delete a directory
    const char* dirname = "path/to/directory";
    if (std::rmdir(dirname) != 0) {
        std::perror("Error deleting the directory");
        return 1;
    }
    else {
        std::puts("Directory deleted successfully");
    }

    return 0;
}