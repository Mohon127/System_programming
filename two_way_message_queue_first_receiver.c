#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct message {
    long type;
    char text[100];
};

int main() {

    //================ Message queue credentials ===============================
    key_t key = 1234;
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    //======================  User prompt ========================================
    char *prompt1 = "Receive  : ";
    char *prompt2 = "Send     : ";

    while (1) {
        struct message receive, send;
        receive.type = 1;
        send.type = 2;

        //===========================   Message receiving part ===============
        if (msgrcv(msqid, &receive, sizeof(receive.text), 1, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        receive.text[sizeof(receive.text) - 1] = '\0'; // safety null-termination
        write(1, prompt1, strlen(prompt1));
        write(1, receive.text, strlen(receive.text));
        write(1, "\n", 1);

        if (strncasecmp(receive.text, "exit", 4) == 0) {
            printf("Peer exited. Closing.\n");
            exit(EXIT_SUCCESS);
        }

        //==============================  message sending part =================
        write(1, prompt2, strlen(prompt2));

        ssize_t len = read(0, send.text, sizeof(send.text) - 1);
        if (len > 0) {
            if (send.text[len - 1] == '\n') {
                send.text[len - 1] = '\0'; // remove newline
            } else {
                send.text[len] = '\0';
            }
        }

        if (msgsnd(msqid, &send, sizeof(send.text), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        if (strncasecmp(send.text, "exit", 4) == 0) {
            if (msgctl(msqid, IPC_RMID, 0) == -1) {
                perror("msgctl");
                exit(EXIT_FAILURE);
            }
            printf("You exited. Message queue removed.\n");
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}




/*
Note:
1. The read() function in C does not automatically add a null-terminator (\0) and
   includes the newline (\n) when the user presses Enter. To safely use the input as a string:
  
    > Remove the newline (\n)
    > Manually add \0 at the end


2. Message Type Design:

| Message Type | Purpose        |
| ------------ | -------------- |
| `1`          | Sender sends   |
| `2`          | Receiver sends |


*/