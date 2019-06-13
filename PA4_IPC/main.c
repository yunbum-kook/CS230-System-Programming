#include <stdio.h>
#include "ipc_test.h"

// Do not modify this file

int main()
{
    char *rcv;
    char *msg;
    msg = (char *) malloc(100);
    strcpy(msg, "asdf");
    
    //Shared Memory
    //rcv = ipc_rcv(1, 23);
    //printf("Data read from memory: %s\n", rcv);
    
    ipc_send(1, msg, 22);
    rcv = ipc_rcv(1, 22);
    printf("Data read from memory: %s %lu\n", rcv, strlen(rcv));
    rcv = ipc_rcv(1, 22);
    printf("Data read from memory: %s\n", rcv);
    free(msg);
    msg = (char *) malloc(100);
    strcpy(msg, "1234567891010 11 ");
    ipc_send(1, msg, 22);
    rcv = ipc_rcv(1, 22);
    printf("Data read from memory: %s %lu\n", rcv, strlen(rcv));
    
    //Message
    //rcv = ipc_rcv(0, 22);
    //printf("Data read from message queue: %s\n", rcv);
    
    
    ipc_send(0, msg, 22);
    rcv = ipc_rcv(0, 22);
    printf("Data read from message queue: %s\n", rcv);
    free(msg);
    msg = (char *) malloc(100);
    strcpy(msg, "888888 999 ");
    ipc_send(0, msg, 23);
    rcv = ipc_rcv(0, 23);
    printf("Data read from message queue: %s %lu\n", rcv, strlen(rcv));
    free(msg);
    msg = (char *) malloc(100);
    strcpy(msg, "888 888 99 9");
    return 0;
}
