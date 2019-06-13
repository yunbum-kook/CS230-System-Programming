#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Create file 
    Be careful when you set permission about your file.
*/
int create_file(char* filename) {
    int fd;
    if((fd = creat(filename, 0644))<0){
        printf("failed to create file");
    }
    return fd;
}

/*
    Read 'size' bytes in file and print it.
    If there is no such file, print ("error occured\n").
*/
void read_file(char* filename, int size) {
    int fd;
    if ((fd = open(filename, O_RDONLY))<0) {
        printf("error occured\n");
        return;
    }
    char buf[size+1];
    int nbytes;
    
    if((nbytes = read(fd, buf, size))<0){
        printf("unable to read file\n");
        close(fd);
    }
    buf[nbytes] = 0;
    printf("%d %s\n",size, buf);
    close(fd);
}

/*
	Open file and write char array tmp to your file.
*/
void write_file(char* filename, char* tmp) {
    int fd;
    int nbytes;
    if((fd = open(filename, O_WRONLY))<0) {
        fd = create_file(filename);
    }
    if ((nbytes = write(fd, tmp, sizeof(tmp)))<0) {
        printf("error on writing\n");
        close(fd);
    }
    close(fd);
}

/* 
    Copy the contents of source file to the dest file back and forth as much as n bytes.For example, suppose "1234abcd\n" is the content of the source file. Then "1a2b3c4d\n" will be the content of the dest file. You sholud use lseek() function.
    If there is no dest file, create it by using "create_file" funtion
*/
void weird_copy_file(char* source, char* dest,int n) {
    int fd1, fd2, dfd;
    char c;
    char buf[512];
    char* tmp = buf;
    if((fd1 = open(source, O_RDONLY))<0){
        printf("no such file\n");
        return;
    }
    if((fd2 = open(source, O_RDONLY))<0){
        printf("no such file\n");
        return;
    }
    lseek(fd2, n, SEEK_SET);
    
    if((dfd = open(dest, O_WRONLY))<0){
        dfd = create_file(dest);
    }
    
    for(int i =0;i<sizeof(source)/2;i++){
        read(fd1, &c, 1);
        memcpy(tmp, &c, 1);
        tmp++;
        read(fd2, &c, 1);
        memcpy(tmp, &c, 1);
        tmp++;
    }
    *tmp = '\0';
    write_file(dest, buf);
    
    close(fd1);
    close(fd2);
    close(dfd);
}

/*
    Get 'command' as an input,
    and execute proper functionalities according to command line.
*/
void file(char*command) {
    char com[100];
    strcpy(com, command);
    
    char* first = strtok(com, " \n");
    char* second = strtok(NULL, " \n");
    
    if(strcmp(first, "create") ==0){
        create_file(second);
    }
    else if(strcmp(first, "write") ==0){
        char* third = strtok(NULL, " \n");
        write_file(second, third);
    }
    else if(strcmp(first, "read") ==0){
        char* third = strtok(NULL, " \n");
        int n = atoi(third);
        read_file(second, n);
    }
    else if(strcmp(first, "copy") ==0){
        char* third = strtok(NULL, " \n");
        char* fourth = strtok(NULL, " \n");
        int n = atoi(fourth);
        weird_copy_file(second, third, n);
    }
    else{}

}

int main(){
    char command[100];  // maximum input char length = 100

	while (1) {
		fgets(command, sizeof(command), stdin); // get command line
		file(command);
	}
}