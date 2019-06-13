#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#define MAXLINE 1024
#define CLIENT "./Client_files/"
#define SERVER "./Server_files/"

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
	char path[100] = SERVER;
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
			strcpy(newpath, SERVER);
			strcat(newpath, fn);
			strcat(newpath, num);
			strcat(newpath, ".");
			strcat(newpath, format);
			dst_fd = open(newpath, O_RDWR | O_CREAT | O_EXCL, 0777);
		}
		
		if (dst_fd == -1) overlap++;
		else break;
	}
	return dst_fd;
}

// In server side, it's simliar to download of client
void upload(int socket, char* filename) {
	int dst_fd, size;
	dst_fd = multiple(filename);
	size = receive_byte(socket);
	while (size > 0) {
		void* buffer = malloc(MAXLINE * sizeof(char));
		read(socket, buffer, MAXLINE);
		write(dst_fd, buffer, MAXLINE);
		free(buffer);
		size -= MAXLINE;
	}
	close(dst_fd);
}

void download(int socket, char* filename) {
	printf("[Request] DOWNLOAD %s\n", filename);
	int src_fd, size;
	char path[100] = SERVER;
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
	printf("DOWNLOAD DONE: %s downloaded in Client_files\n", filename);
}

int open_listenfd(char* port) {
	struct addrinfo hints, *listp, *p;
	int listenfd, optval = 1;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	hints.ai_flags |= AI_NUMERICSERV;
	getaddrinfo(NULL, port, &hints, &listp);

	for (p = listp; p; p = p->ai_next) {
		if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
			continue;
		}
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int));
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
			break;
		}
		close(listenfd);
	} 
	freeaddrinfo(listp);
	if (!p)
		return -1;

	if (listen(listenfd, 8) < 0) {
		close(listenfd);
		return -1;
	}
	return listenfd;
}

int main(int argc, char* argv[]) {
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];
	
	char* port = argv[2];
	listenfd = open_listenfd(port);

	printf("Server start: listen on port %s\n", port);
	clientlen = sizeof(struct sockaddr_storage);
	connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &clientlen);
	printf("Connected\n");
	while (1) {
		char buf[100];

		// Receive byte of command & command itself
		int rb = receive_byte(connfd);
		int rd = read(connfd, buf, rb); //strlen(buf) > rd???
		buf[rd] = '\0';
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
			close(connfd);
			exit(0);
		}
		else if (!strcmp(command, "upload")) {
			printf("[Request] UPLOAD %s\n", filename);
			upload(connfd, filename);
			printf("UPLOAD DONE: %s uploaded in Server_files\n", filename);	
		}
		else if (!strcmp(command, "download")) {
			int exist = check_file(filename);
			if (exist == 0) {
				write(connfd, "N", 1);
				continue;
			}
			else write(connfd, "Y", 1);
			download(connfd, filename);
		}
	}
	close(connfd);
	return 0;
}