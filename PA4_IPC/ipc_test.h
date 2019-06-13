#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/msg.h> 
#include <sys/types.h>

enum IPC_TYPE{MSG_QUEUE, SHM_MEM};
#define PATH_NAME   "CS230"

// Struct for message queue 
struct mesg_buffer { 
  long mesg_type; 
  char *mesg_text; 
};

/* Function prototypes */
// Message Queue
void msg_queue_send(char *msg, int prj_id);
char* msg_queue_rcv(int prj_id);
// Shared Memory
void shm_mem_send(char *msg, int prj_id);
char* shm_mem_rcv(int prj_id);
// General
void ipc_send(int ipc_type, char *msg, int prj_id);
char* ipc_rcv(int ipc_type, int prj_id);
