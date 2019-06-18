#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>

/*
    Create file 
    Be careful when you set permission about your file.
*/
int create_file(char* filename) {
    int fd =  creat(filename, 0777);
    return fd;
}

/*
    Read 'size' bytes in file and print it.
    If there is no such file, print ("error occured\n").
*/
void read_file(char* filename, int size) {
    int fd;
    if ((fd = open(filename, O_RDONLY, 0777)) < 0){
        printf("error occured\n");
    } else{
        char *buf = (char *) malloc(sizeof(char) * (size + 1));
        int byte = read(fd, buf, size);
        buf[byte] = '\0';
        if (buf[strlen(buf)-1] == '\n'){
            byte--;
            buf[strlen(buf)-1] = '\0';
        }
        printf("%d %s\n", byte, buf);
        free(buf);
        close(fd);
    }
}

/*
	Open file and write char array tmp to your file.
*/
void write_file(char* filename, char* tmp) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    write(fd, tmp, strlen(tmp));
    close(fd);
}

/* 
    Copy the contents of source file to the dest file back and forth as much as n bytes.For example, suppose "1234abcd\n" is the content of the source file. Then "1a2b3c4d\n" will be the content of the dest file. You sholud use lseek() function.
    If there is no dest file, create it by using "create_file" funtion
*/
void weird_copy_file(char* source, char* dest, int n) {
    int sour_fd = open(source, O_RDONLY, 0777);
    int dest_fd;
    
    if ((dest_fd = open(dest, O_RDWR | O_TRUNC, 0777)) < 0){
        create_file(dest);
    }
    dest_fd = open(dest, O_RDWR | O_TRUNC, 0777);
    for (int i=0; i<n; i++){
        char buf[3];
        lseek(sour_fd, i, SEEK_SET);
        read(sour_fd, buf, 1);
        write(dest_fd, buf, 1);
        
        lseek(sour_fd, n+i, SEEK_SET);
        read(sour_fd, buf, 1);
        write(dest_fd, buf, 1);
    }
    char buf[2];
    lseek(sour_fd, -1, SEEK_END);
    read(sour_fd, buf, 1);
    if (buf[0] == '\n'){
        write(dest_fd, "\n", 1);
    }
    close(sour_fd);
    close(dest_fd);
}

/*
    Get 'command' as an input,
    and execute proper functionalities according to command line.
*/
void file(char* command) {
    char command_copy[100];
    strcpy(command_copy, command);
    char *ptr = strtok(command_copy, " ");
    char *f_name = strtok(NULL, " ");
    
    if (!strcmp(ptr, "create")){
        if (f_name[strlen(f_name)-1] == '\n'){
            f_name[strlen(f_name)-1] = '\0';
        }
        create_file(f_name);
    }else if (!strcmp(ptr, "read")){
        int num = atoi(strtok(NULL, " "));
        read_file(f_name, num);
    }else if (!strcmp(ptr, "write")){
        char *tmp = strtok(NULL, " ");
        if (tmp[strlen(tmp)-1] == '\n'){
            tmp[strlen(tmp)-1] = '\0';
        }
        write_file(f_name, tmp);
    }else if (!strcmp(ptr, "copy")){
        char *source = strtok(NULL, " ");
        int num = atoi(strtok(NULL, " "));
        weird_copy_file(f_name, source, num);
    }
}