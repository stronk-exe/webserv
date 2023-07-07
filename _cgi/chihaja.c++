#include <fcntl.h>
#include <unistd.h>

size_t modifyFile(int fd, off_t offset, std::string newData) {
    ;
    if (fd == -1) {
        perror("open");
        return;
    }

    // Move the file pointer to the desired position
    off_t result = lseek(fd, offset, SEEK_SET);
    if (result == -1) {
        perror("lseek");
        close(fd);
        return;
    }

    // Write the new data at the specified position
    size_t bytesWritten = write(fd, newData.c_str(), newData.size());
    if (bytesWritten == -1) {
        perror("write");
        // close(fd);
        return 0;
    }

    // Close the file
    // close(fd);
	return bytesWritten;
}