/*
Objective:
Design and implement two unrelated C programs that communicate with each other using
named pipes (FIFOs). The communication should be two-way, allowing both processes to 
send and receive messages.

Requirements:

    > Use mkfifo() in the C code to create two named pipes: fifo1 and fifo2.

    > Process A should write to fifo1 and read from fifo2.

    > Process B should read from fifo1 and write to fifo2.

    > Messages should be exchanged in a loop until one process sends the message "exit".

    > Ensure proper buffer handling and clean termination.

Note: Before starting execution create two fifo in current directory using the following command:
    > mkfifo fifo1
    > mkfifo fifo2

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX 100

int main() {
    int fd_write, fd_read;
    char write_msg[MAX], read_msg[MAX];

    // Open fifo1 for writing, fifo2 for reading
    fd_write = open("fifo1", O_WRONLY);
    fd_read = open("fifo2", O_RDONLY);

    if (fd_write < 0 || fd_read < 0) {
        perror("open");
        exit(1);
    }

    while (1) {
        printf("[A] Enter message: ");
        fgets(write_msg, MAX, stdin);
        write(fd_write, write_msg, strlen(write_msg) + 1);

        if (strncasecmp(write_msg, "exit", 4) == 0) break;

        read(fd_read, read_msg, MAX);
        printf("[A] Received: %s\n", read_msg);
        if (strncasecmp(read_msg, "exit", 4) == 0) break;
    }

    close(fd_write);
    close(fd_read);
    return 0;
}
