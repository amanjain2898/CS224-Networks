
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
#include <map>
#include <vector>

using namespace std;

#define BACKLOG 10
#define MAXSIZE 1000

long long int min(long long int a,long long int b)
{
	return (a<b)?a:b;
}

struct connect_identifier
{
	char* filename;
	long long int bytes_sent;
	FILE* file_pointer;
	connect_identifier()
	{
		filename = '\0';
		bytes_sent = 0;
	}
	connect_identifier(char* f,long long int bytes,long long int buffer_size)
	{
		filename = new char[buffer_size];
		for(int i=0;i<buffer_size;i++)
		{
			filename[i] = f[i];
		}
		filename[buffer_size] = '\0';
		bytes_sent = bytes;
	}
	void incr(int x)
	{
		bytes_sent+=x;
	}
	void set_filename(char* filename)
	{
		this->filename = filename;
	}
};

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

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 500000;

    int fdmax;
    map<int,connect_identifier> clients_table;
    fd_set master_read;
    fd_set master_write;
    fd_set read_fds;
    fd_set write_fds;

    FD_ZERO(&master_read);
    FD_ZERO(&master_write);
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

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

	FD_SET(sockfd,&master_read);
	fdmax = sockfd;

	while(1)
	{
		read_fds = master_read;
		write_fds = master_write;

		if(select(fdmax+1,&read_fds,&write_fds,NULL,NULL) == -1)
		{
			perror("select");
			exit(25);
		}

		//cout<<"here"<<endl;

		for(int i=0;i<=fdmax;i++)
		{
			if(FD_ISSET(i,&read_fds)) // checking for reading from a socket
			{
				if(i == sockfd) // checking for new connection 
				{
					sin_size = sizeof(struct sockaddr_in);
		            new_fd = accept(sockfd, (struct sockaddr *)&connector_addr, &sin_size);
		            if(new_fd < 0) {perror("connection not accepted");}
		            else 
		            {
		              cout<<"Client: "<<inet_ntoa(connector_addr.sin_addr)<<":"<<ntohs(connector_addr.sin_port)<<"\n";
		              FD_SET(new_fd,&master_read);
		              if(new_fd > fdmax)
		              {
		              	fdmax = new_fd;
		              }
		            }
				}
				else
				{
					if(clients_table.find(i) == clients_table.end())
					{
						// receive data from already existing client
						memset(&receive_buffer,'\0',MAXSIZE);
						recv_check = recv(i , receive_buffer, MAXSIZE, 0);

						if(recv_check <= 0)
						{
							// got error or connection closed by connection
							if(recv_check == 0) cout<<"connection closed by client"<<endl;
							else cout<<"recv error"<<endl;
						}
						else 
						{
						    int l = recv_check;
						    char filename[l-4];
						    memset(&filename,'\0',l-4);

						    if( !(((receive_buffer[0] == 'g') &&
						    	 (receive_buffer[1] == 'e') &&
						    	 (receive_buffer[2] == 't') &&
						    	 (receive_buffer[3] == ' ')) ||

						    	 ((receive_buffer[0] == 'p') &&
						    	 (receive_buffer[1] == 'u') &&
						    	 (receive_buffer[2] == 't') &&
						    	 (receive_buffer[3] == ' '))))
						    {
							    cout<<"UnknownCmd\n";
								fprintf(stderr,"%s","Unknown Command given");
								close(i);
								FD_CLR(i,&master_read);
							}
							else 
	        				{
	        					//cout<<recv_check<<endl;
	        					int x = 0;
				    			for(;x<recv_check;x++)
				    			{
				    				if(x > 3)
				    				{
				    					if (receive_buffer[x] == '\0') break;
				    					filename[x-4] = receive_buffer[x];
				    				}
				    			}
				    			filename[x-4] = '\0';
				    			//cout<<filename<<endl;
				    			//cout<<"here"<<endl;

				    			char remaining_buffer[recv_check-x-1];
				    			for(int y=x+1;y<recv_check;y++)
				    			{
				    				remaining_buffer[y-x-1] = receive_buffer[y];
				    			}
				    			//remaining_buffer[recv_check-x] = '\0';

				    			if( ((receive_buffer[0] == 'g') &&
						    	     (receive_buffer[1] == 'e') &&
						    	     (receive_buffer[2] == 't') &&
						    	     (receive_buffer[3] == ' ')) )
				    			{
				    				cout<<"FileRequested: "<<filename<<endl;
				    				connect_identifier connection(filename,0,strlen(filename));
				    				clients_table[i] = connection;
				    				FD_CLR(i,&master_read);
				    				FD_SET(i,&master_write);
				    			}
				    			else 
				    			{
				    				//cout<<receive_buffer<<endl;
				    				//cout<<"FileToBeWritten: "<<filename<<endl;
				    				connect_identifier connection(filename,0,strlen(filename));
				    				FILE* f = fopen(filename,"w");
				    				if(f)
				    				{
				    					connection.file_pointer = f;
				    					if(recv_check-x-1 > 0)
				    					{
				    					fwrite(remaining_buffer,1,recv_check-x,f);
				    					connection.bytes_sent+=recv_check-x-1;
				    					}
				    					clients_table[i] = connection;
				    					//cout<<f<<endl;
				    				}
				    				else
				    				{
				    					perror("Unable to open file");
				    				}
				    			}						
							}
						}
					}
					else
					{
						long long int bytes_written_until_now = clients_table[i].bytes_sent;

						memset(receive_buffer,'\0',MAXSIZE);
						recv_check = recv(i,receive_buffer,MAXSIZE,0);
						if(recv_check < 0) {perror("recv error");}
						// cout<<"Received "<<recv_check<<" bytes"<<endl;
						else if(recv_check == 0)
						{
							cout<<"FileWritten: "<<bytes_written_until_now<<" bytes\n";
							FD_CLR(i,&master_read);
							close(i);
							// cout<<clients_table[i].file_pointer<<endl;
							fclose(clients_table[i].file_pointer);
						    clients_table.erase(i);
						}
						else
						{
							//cout<<clients_table[i].file_pointer<<endl;
							fwrite(receive_buffer,1,recv_check,clients_table[i].file_pointer);
							clients_table[i].bytes_sent += recv_check;
						}
					}	
				}
			}

			else if(FD_ISSET(i,&write_fds))
			{
				//char* filename = clients_table[i].filename;
				long long int bytes_sent_untilnow = clients_table[i].bytes_sent;
				long long int end_file;

				ifstream File(clients_table[i].filename);
				if(!File.good()) 
				{ 
					cout<<"FileTransferFail\n";
		            fprintf(stderr,"%s","File not present or not readable");
		            close(i);
		            clients_table.erase(i);
		            FD_CLR(i,&master_write);
	            }
	            else
	            {
	            	File.seekg(0,File.end);
	            	end_file = File.tellg();
	            	File.seekg(0,File.beg);

	            	File.seekg(bytes_sent_untilnow,File.beg);

	            	long long int length_remaining = end_file - bytes_sent_untilnow;
	            	long long int num_bytes_to_send = min(MAXSIZE,length_remaining);

	            	if(num_bytes_to_send > 0)
	            	{
	            		memset(&send_buffer,'\0',MAXSIZE);
						File.read(send_buffer,num_bytes_to_send);
						bytes_sent = send(i,&send_buffer,num_bytes_to_send,0);
						clients_table[i].bytes_sent += bytes_sent;
	            	}
	            	else
	            	{
	            		cout<<"TransferDone: "<<bytes_sent_untilnow<<" bytes"<<"\n";
	            		close(i);
	            		clients_table.erase(i);
	            		FD_CLR(i,&master_write);
	            	}
	            }
			}
		}

	}

    close(sockfd);

	return 0;
}