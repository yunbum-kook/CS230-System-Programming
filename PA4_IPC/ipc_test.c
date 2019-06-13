#include "ipc_test.h"

/*
    Send a message via message queue
    msg: Message to be sent
    prj_id: ID to create unique ipc key
*/
void msg_queue_send(char *msg, int prj_id) {
    int q_id;
    
    key_t msg_key = ftok("./CS230", prj_id);
    printf("Generated key is: %d\n", msg_key);
    
    if ((q_id = msgget(msg_key, IPC_CREAT | 0666)) < 0){
        printf("Error in message queue ID creation, SND");
        //return -1;
        exit(1);
    }
    
    struct mesg_buffer message;
    message.mesg_type = (long) 1;
    message.mesg_text = msg;
    
    if (msgsnd(q_id, (void *) &message, sizeof(msg), IPC_NOWAIT) < 0){
        printf("Error in pushing message into queue, SND");
        //return -1;
        exit(1);
    }else{
        printf("Data written in message queue: %s\n", message.mesg_text);   
    }
}

/*
    Receive a message via message queue
    The program may hang due to no data, consider to handle this case
    Be sure the IPC keys of two process are them same
    Expected output: Message obtained from queue
*/
char* msg_queue_rcv(int prj_id) {
    int q_id;
    struct mesg_buffer message;
    
    key_t msg_key = ftok("./CS230", prj_id);
    printf("Generated key is: %d\n", msg_key);
    
    if ((q_id = msgget(msg_key, IPC_CREAT | 0666)) < 0){
        printf("Error in message queue ID creation, RCV");
        return NULL;
        //exit(1);
    }
    
    if (msgrcv(q_id, (void *) &message, sizeof(struct mesg_buffer), (long) 1, IPC_NOWAIT) < 0){
        printf("Nothing to receive");
        //exit(1);
        return NULL;
    }
    return message.mesg_text;
}

/*
    Send a message via shared memory
*/
void shm_mem_send(char *msg, int prj_id) {
    int mem_id;
    int q_id;
    void *sm_add = NULL; /* void *??? */
    
    key_t msg_key = ftok("./CS230", prj_id);
    printf("Generated key is: %d\n", msg_key);
    
    if ((mem_id = shmget(msg_key, 1024, IPC_CREAT | 0666)) < 0){
        printf("Error in getting shared memory ID, SND");
        return;
    }
    
    if ((sm_add = shmat(mem_id, NULL, 0)) == (void *) -1){
        printf("Error in shmat");
        return;
    }else{
        memcpy(sm_add, msg, strlen(msg));
        printf("Data written in memory: %s\n", (char *) sm_add);
    }
}

/*
    Receive a message via shared memory
    The program may hang due to no data, consider to handle this case
    Expected output: Message obtained from shared memory
*/
char* shm_mem_rcv(int prj_id) {
    int mem_id;
    int q_id;
    void *sm_add = NULL; /* void *??? */
    
    key_t msg_key = ftok("./CS230", prj_id);
    printf("Generated key is: %d\n", msg_key);
    
    if ((mem_id = shmget(msg_key, 1024, IPC_CREAT | 0666)) < 0){
        printf("Error in getting shared memory ID, SND\n");
        return NULL;
    }
    
    if ((sm_add = shmat(mem_id, NULL, 0)) == ((void *) -1)){
        printf("Error in shmat");
        return NULL;
    }
    char * str = (char *) shmat(mem_id, NULL, 0);
    if (*str == 0){
        printf("Null string");
        return NULL;
    }

    if (shmdt(sm_add) == -1){
        return NULL;
    }
    shmctl(mem_id, IPC_RMID, NULL); 
    return str;
}

/*
    General function for IPC sending
    Recommended to not modify this function
*/
void ipc_send(int ipc_type, char *msg, int prj_id) {
    switch (ipc_type) {
    case MSG_QUEUE:
        msg_queue_send(msg, prj_id);
        break;
    case SHM_MEM:
        shm_mem_send(msg, prj_id);
        break;
    default:
        printf("Wrong IPC type! \n");
        break;
    }
}

/*
    General function for IPC receiving
    Recommended to not modify this function
*/
char* ipc_rcv(int ipc_type, int prj_id) {
    char* rcv = NULL;
    
    switch (ipc_type) {
    case MSG_QUEUE:
        rcv = msg_queue_rcv(prj_id);
        break;
    case SHM_MEM:
        rcv = shm_mem_rcv(prj_id);
        break;
    default:
        printf("Wrong IPC type! \n");
        break;
    }
    
    return rcv;
}
