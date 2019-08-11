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

long long int min(long long int a,long long int b)
{
	return (a<b)?a:b;
}

/* Print usage of programe */
void usage (char* progname)
{
	fprintf(stderr,"%s <portNum>\n",progname);
} // End usage

int main(int argc,char *argv[])
{
	int portNum;
	char fileToTransfer[MAXSIZE];
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct sockaddr_in my_addr; // my address information
	struct sockaddr_in connector_addr; // connector's address information
	int bind_socket;
	int listen_socket;
	socklen_t sin_size;
	long long int bytes_sent;
	int length_message;
	long long int total_bytes_sent = 0;
	char receive_buffer[MAXSIZE];
	char send_buffer[MAXSIZE];
    char byte;
    int recv_check;

	/* Get command-line arguments */
	if(argc != 2) { usage(argv[0]); exit(1); }
	portNum = atoi(argv[1]);

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

	listen_socket = listen(sockfd,BACKLOG);
	if(listen_socket < 0) { perror("listen failed"); exit(21); }
	else {cout<<"ListenDone: "<<portNum<<"\n";}

	sin_size = sizeof(struct sockaddr_in);

	while(1)
	{
		total_bytes_sent = 0;
		new_fd = accept(sockfd, (struct sockaddr *)&connector_addr, &sin_size);
		if(new_fd < 0) {perror("connection not accepted"); continue;}
		else {cout<<"Client: "<<inet_ntoa(connector_addr.sin_addr)<<":"<<ntohs(connector_addr.sin_port)<<"\n";}

		memset(&receive_buffer,'\0',MAXSIZE);
		recv_check = recv(new_fd , receive_buffer, MAXSIZE, 0);
		//cout<<receive_buffer<<endl;

	    int l = strlen(receive_buffer);
	    char filename[l-4];

	    if(! ((receive_buffer[0] == 'g') &&
	    	 (receive_buffer[1] == 'e') &&
	    	 (receive_buffer[2] == 't') &&
	    	 (receive_buffer[3] == ' ')))
	    {cout<<"UnknownCmd\n";
		 fprintf(stderr,"%s","Unknown Command given");
		 close(new_fd);
		 continue;
        }
        else 
        {
		    for(int i=0;i<l;i++)
		    {
		    	if(i > 3) filename[i-4] = receive_buffer[i];
		    }
		    filename[l-4] = '\0';

		    cout<<"FileRequested: "<<filename<<endl;

			/* Opening the given file and storing it into character array */
			ifstream File(filename);                                         
			if(!File.good()) 
				{ cout<<"FileTransferFail\n";
	              fprintf(stderr,"%s","File not present or not readable");
	              close(new_fd);
	              continue;
	            }

	        else
	        {
			    // while(File.get(byte))
			    // {
			   	//   bytes_sent = send(new_fd,&byte,1,0);
			   	//   total_bytes_sent += bytes_sent;
			    // }

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
			}
		}
	}

    close(sockfd);

	return 0;
}