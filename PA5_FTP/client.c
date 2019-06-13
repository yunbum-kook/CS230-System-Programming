#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#define MAXLINE 1024
#define CLIENT "./Client_files/"

union {
	unsigned int integer;
	unsigned char byte[4];
} number;

void send_size(int socket, int size) {
	number.integer = size;
	write(socket, number.byte ,4);
}

int receive_byte(int socket) {
	unsigned char buf[4];
	read(socket, buf, 4);
	number.byte[0] = buf[0];
	number.byte[1] = buf[1];
	number.byte[2] = buf[2];
	number.byte[3] = buf[3];

	return number.integer;
}

int check_file(char* filename) {
	char path[100] = CLIENT;
	strcat(path, filename);
	int fd = open(path, O_RDONLY);

	if (fd < 0) {
		return 0;
	}
	return 1;
}

int multiple(char* filename) {
	int dst_fd;
	char path[100] = CLIENT;
	strcat(path, filename);
	int overlap = 1;

	char copyname[50];
	strcpy(copyname, filename);

	char* fn = strtok(copyname, ".");
	char* format = strtok(NULL, " ");

	while (1) {
		if (overlap == 1) {
			dst_fd = open(path, O_RDWR | O_CREAT | O_EXCL, 0777);
		} else {
			char newpath[100], num[7];
			sprintf(num, "%d", overlap);
			strcpy(newpath, CLIENT);
			strcat(newpath, fn);
			strcat(newpath, num);
			strcat(newpath, ".");
			strcat(newpath, format);
			
			dst_fd = open(newpath, O_RDWR | O_CREAT | O_EXCL, 0777);
		}
		
		if (dst_fd == -1) {
			overlap++;
		} else {
			break;
		}
	}
	return dst_fd;
}

/*
 * DESCRIPTION: List files in "client_files" directory
 * 
 * PARAMETERS: None
 * RETURNS: None
 */
void list() {
	system("/bin/ls ./Client_files");
}

/*
 * DESCRIPTION: Upload the file with FILENAME to the server 
 * 
 * PARAMETERS: 
 *  - int socket: 
 *  - char* filename: name of the file to upload (in client_file directory)
 * 
 * RETURNS: None
 * 
 * NOTES: If no such file exists, then print "Error: no such file" in stdout
 */
void upload(int socket, char* filename) {
	int exist = check_file(filename);
	if (exist == 0) {
		printf("Error: no such file\n");
		return;
	}

	// Send the command size & command itself
	char copybuf[100];
	strcpy(copybuf, "upload ");
	strcat(copybuf, filename);
	send_size(socket, (int) strlen(copybuf));
	write(socket, copybuf, strlen(copybuf));
	
	// Initialization
	int src_fd, size;
	char path[100] = CLIENT;
	strcat(path, filename);
	src_fd = open(path, O_RDONLY);
	
	// Size of file
	struct stat st;
	stat(path, &st);
	char ssize[10];
	size = (int) st.st_size;
	
	// Send the file size & file content
	send_size(socket, size);
	while (size > 0) {
		void* buffer = malloc(MAXLINE * sizeof(char));
		read(src_fd, buffer, MAXLINE);
		write(socket, buffer, MAXLINE);
		free(buffer);
		size -= MAXLINE;
	}
	close(src_fd);
	printf("UPLOAD DONE\n");
}

/*
 * DESCRIPTION: Download the file with FILENAME from the server 
 * 
 * PARAMETERS: 
 *  - int socket: 
 *  - char* filename: name of the file to download (in server_file directory)
 * 
 * RETURNS: None
 * 
 * NOTES: If no such file exists, then print "Error: no such file" in stdout
 */

void download(int socket, char* filename) {
	// Send the command size & command itself
	char copybuf[100];
	strcpy(copybuf, "download ");
	strcat(copybuf, filename);

	send_size(socket, (int) strlen(copybuf));
	write(socket, copybuf, strlen(copybuf));

	// Existence of the file in the server
	char indicator[2];
	read(socket, indicator, 1);
	if (!strcmp(indicator, "N")) {
		printf("Error: no such file\n");
		return;
	}
	int dst_fd, size;
	dst_fd = multiple(filename);
	
	// Receive the file size & file content
	size = receive_byte(socket);
	while (size > 0) {
		void* buffer = malloc(MAXLINE * sizeof(char));
		read(socket, buffer, MAXLINE);
		write(dst_fd, buffer, MAXLINE);
		free(buffer);
		size -= MAXLINE;
	}
	close(dst_fd);
	printf("DOWNLOAD DONE\n");
}

int open_clientfd(char *host, char *port) {
	if (strcmp(host, "127.0.0.1")) return -1;
	int clientfd;
	struct addrinfo hints, *listp, *p;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	hints.ai_flags |= AI_NUMERICSERV;
	getaddrinfo(host, port, &hints, &listp);

	for (p = listp; p; p = p->ai_next) {
		if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
			continue;
		}
		
		if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) {
			break;
		}
		close(clientfd);
	} 
	
	freeaddrinfo(listp);
	if (!p)
		return -1;
	else
		return clientfd;
}

int main(int argc, char* argv[]) {
	int clientfd;
	char *host = argv[2];
	char *port = argv[4];
	
	if ((clientfd = open_clientfd(host, port)) < 0) {
		printf("Invalid IP address or port number\n");
		exit(1);
	}
	
	while (1) {
		printf("[20150056]> ");
		char buf[100];

		if (fgets(buf, MAXLINE, stdin) <= 0) break;

		char* command = strtok(buf, " ");
		char* filename = strtok(NULL, " ");

		// Processing command and filename
		if (command[strlen(command) - 1] == '\n') {
			command[strlen(command) - 1] = '\0';
		}
		if (filename != 0 && filename[strlen(filename) - 1] == '\n'){
			filename[strlen(filename) - 1] = '\0';
		}

		// Classify command (request)
		if (!strcmp(command, "exit")) {
			send_size(clientfd, 4);
			write(clientfd, "exit", 4);
			close(clientfd);
			exit(0);
		}
		else if (!strcmp(command, "ls")) {
			list();
		}
		else if (!strcmp(command, "upload")) {
			upload(clientfd, filename);
		}
		else if (!strcmp(command, "download")){
			download(clientfd, filename);
		}
		else {
			printf("Invalid Command\n");
		}
	}
	close(clientfd);
	return 0;
}