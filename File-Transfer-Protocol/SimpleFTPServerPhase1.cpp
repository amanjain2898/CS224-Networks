#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fstream>

using namespace std;

#define BACKLOG 10
#define MAXSIZE 1024

/* Print usage of programe */
void usage (char* progname)
{
	fprintf(stderr,"%s <portNum> <fileToTransfer>\n",progname);
} // End usage

long long int min (long long int a,long long int b)
{
	return (a<b)?a:b;
}

int main(int argc,char *argv[])
{
	int portNum;
	char* fileToTransfer;
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct sockaddr_in my_addr; // my address information
	struct sockaddr_in connector_addr; // connector's address information
	int bind_socket;
	int listen_socket;
	socklen_t sin_size;
	long long int bytes_sent;
	int length_message;
	long long int total_bytes_sent = 0;
    char byte;
    char send_buffer[MAXSIZE];
    memset(&send_buffer,'\0',MAXSIZE);

	/* Get command-line arguments */
	if(argc != 3) { usage(argv[0]); exit(1); }
	portNum = atoi(argv[1]);
    fileToTransfer = argv[2];

	/* socket */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) { perror("socket failed"); exit(20); }
    
    /* filling the fields of the server side */
    my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(portNum);
	my_addr.sin_addr.s_addr = INADDR_ANY; // auto-fill with my IP
	memset(&(my_addr.sin_zero), '\0', 8); // set to zero the rest of the struct

	/* Binding the socket */
	bind_socket = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
	if(bind_socket < 0) { perror("bind failed"); exit(2); }
	else {cout<<"BindDone: "<<portNum<<"\n";}

	/* Opening the given file and storing it into character array */
	ifstream File(fileToTransfer);                                         
	if(!File.good()) { perror("File not present or not readable"); exit(3); }

	listen_socket = listen(sockfd,BACKLOG);
	if(listen_socket < 0) { perror("listen failed"); exit(21); }
	else {cout<<"ListenDone: "<<portNum<<"\n";}

	sin_size = sizeof(struct sockaddr_in);

	new_fd = accept(sockfd, (struct sockaddr *)&connector_addr, &sin_size);
	if(new_fd < 0) {perror("connection not accepted"); exit(22);}
	else {cout<<"Client: "<<inet_ntoa(connector_addr.sin_addr)<<":"<<ntohs(connector_addr.sin_port)<<"\n";}

  //   while(File.get(byte))
  //   {
		// bytes_sent = send(new_fd,&byte,1,0);
		// total_bytes_sent += bytes_sent;
  //   }

	File.seekg(0,File.end);
	long long int end_pos = File.tellg();
	File.seekg(0,File.beg);
	long long int length = end_pos;

	while(length > 0)
	{
		long long int current_pos = File.tellg();
		long long int length_remaining = end_pos - current_pos;
		long long int num_bytes_to_send = min(MAXSIZE,length_remaining);
		File.read(send_buffer,num_bytes_to_send);

		bytes_sent = send(new_fd,&send_buffer,num_bytes_to_send,0);
		
		total_bytes_sent += bytes_sent;
		length -= bytes_sent;
	}

    cout<<"TransferDone: "<<total_bytes_sent<<" bytes"<<"\n";

    close(new_fd);
    close(sockfd);

	return 0;
}