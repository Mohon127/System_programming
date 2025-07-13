#include <stdlib.h>
#include <sys/ipc.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <stdio.h>

struct message {
    long type;
    char text[100];
};

int main() {
    //=============== Message queue credentials ==========================
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

    //======================  User prompt ================================
    char *prompt1 = "Send    :  ";
    char *prompt2 = "Receive :  "; 

    while (1) {
        struct message send, receive;
        send.type = 1;
        receive.type = 2;

        //======================= Message sending part ===================
        write(1, prompt1, strlen(prompt1));
        ssize_t len = read(0, send.text, sizeof(send.text) - 1);

        if (len > 0) {
            if (send.text[len - 1] == '\n') {
                send.text[len - 1] = '\0';
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
            }
            printf("You exited. Message queue removed.\n");
            exit(EXIT_SUCCESS);
        }

        //====================== Message receiving part ==================
        if (msgrcv(msqid, &receive, sizeof(receive.text), 2, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        receive.text[sizeof(receive.text) - 1] = '\0'; // safety
        write(1, prompt2, strlen(prompt2));
        write(1, receive.text, strlen(receive.text));
        write(1, "\n", 1);

        if (strncasecmp(receive.text, "exit", 4) == 0) {
            printf("Peer exited. Closing.\n");
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
