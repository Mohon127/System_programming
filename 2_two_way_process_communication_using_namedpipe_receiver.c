#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX 100

int main() {
    int fd_read, fd_write;
    char write_msg[MAX], read_msg[MAX];

    // Open fifo1 for reading, fifo2 for writing
    fd_read = open("fifo1", O_RDONLY);
    fd_write = open("fifo2", O_WRONLY);

    if (fd_read < 0 || fd_write < 0) {
        perror("open");
        exit(1);
    }

    while (1) {
        read(fd_read, read_msg, MAX);
        printf("[B] Received: %s\n", read_msg);
        if (strncasecmp(read_msg, "exit", 4) == 0) break;

        printf("[B] Enter reply: ");
        fgets(write_msg, MAX, stdin);
        write(fd_write, write_msg, strlen(write_msg) + 1);
        if (strncasecmp(write_msg, "exit", 4) == 0) break;
    }

    close(fd_read);
    close(fd_write);
    return 0;
}
