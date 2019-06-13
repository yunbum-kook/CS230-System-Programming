PA5 File Transfer Protocol - Documentation


1.	open_listenfd and open_clientfd, which conducts all underlying socket formulation such as bind, listen, accept, and connect, are based on Pf. Kim’s PPT.

2.	main() from client and server : It reads in command lines and parses the command line by strtok(). For safety, it always removes ‘\n’ at the end of each command line. According to predefined command such as ls, upload, download, and exit, it calls helper function for each functionality. If the given command is not in the set of predefined commands, it prints out “Invalid Command”, asking for new command line.

3.	Before sending request from client to server, it always send the total bytes of the request by translating the total bytes into 4 bytes. For example, if the command is “upload Sample.mp3”, then the total bytes of the request are 17 bytes. Since integer type value (17 in this case) can be encoded in 4 bytes, this mechanism (sending the total bytes of the request before sending the request) consistently works. Using send_size() and receive_byte(), the server and client can pass the information on how many bytes should be read.

4.	list() of client: Using the system call system(), it lists all files in a directory.

5.	upload() of client & download() of server : Note that the counterpart of upload() of client is download() of server. First of all, using a helper function check_file(), which returns 1 if the file exists or 0 otherwise, upload() considers only existing files. Like I explained in #3, it sends the total bytes of the request “upload [filename]” and the actual request. Then, send the total size of the uploaded file and the actual file separated by up to 1024 bytes. (I tried to send it by using void* buffer and malloc, but the client process terminates for unknown reasons). The counterpart function, download() of server, receives the size of the request and gets ready to the command line “upload [filename]”. With knowledge of the total size of receiving file, it receives the actual file separated by up to 1024 bytes.

6.	download() of client & upload() of server : Note that the counterpart of download() of client is upload() of server. Every process is the same until sending request. In this case, the server should check whether the requested file exists in Server_files directory. To this end, the server and the client give and receive 1 bytes(“Y” or “N”), which is sort of indicator for the existence of the requested file. If the request file exists, the other process is the opposite of #5 except for the handling of overlapped file names. To handle this case, used is a helper function multiple(), which creates a non-overlapped file with manipulation of file name. For example, if “Sample.mp3” file already exists in Client_files, multiple() creates “Sample2.mp3”.

