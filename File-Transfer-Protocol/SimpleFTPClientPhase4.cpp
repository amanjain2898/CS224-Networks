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
#include <sys/time.h>
#include <fstream>

using namespace std;

#define MAXSIZE 1000

long long int min(long long int a,long long int b)
{
  return (a<b)?a:b;
}

/* Print usage of program */
void usage(char *progname) {
  fprintf(stderr, "%s <serverIPAddr:port> <op> <filename> <receiveInternal>\n",
      progname);
} // End usage()

int main(int argc, char *argv[])
{

  /* The socket descriptor */
  int sockfd;
  /* Server IP address and Port Information */
  char* serverIPAddr_port;
  /* To store Destination addr */
  struct sockaddr_in dest_addr;
  /* buffer to receive data */
  char receive_buffer[MAXSIZE];
  /* buffer to send data */
  char send_buffer[MAXSIZE];
  /* File to receive */
  int connect_check;
  int recv_check;
  int sendname;
  int bytes_sent;
  
  /* Get command-line arguments */
  if(argc != 5) { usage(argv[0]); exit(1); }

  serverIPAddr_port = argv[1];
  int len = strlen(argv[3]);
  char filename[len+4];

  char opt[4];
  opt[0] = argv[2][0];
  opt[1] = argv[2][1];
  opt[2] = argv[2][2];
  opt[3] = '\0';

  if((opt[0] == 'g') &&
     (opt[1] == 'e') &&
     (opt[2] == 't'))
  {
    filename[0] = 'g';
    filename[1] = 'e';
    filename[2] = 't';
    filename[3] = ' ';
    for(int i=4;i<len+4;i++)
    {
      filename[i] = argv[3][i-4];
    }
    filename[len+4] = '\0';
  }
  else if((opt[0] == 'p') &&
     (opt[1] == 'u') &&
     (opt[2] == 't'))
  {
    filename[0] = 'p';
    filename[1] = 'u';
    filename[2] = 't';
    filename[3] = ' ';
    for(int i=4;i<len+4;i++)
    {
      filename[i] = argv[3][i-4];
    }
    filename[len+4] = '\0';
  }
  else
  {
    fprintf(stderr,"Invalid Request\n");
    exit(22);
  }

  int i=0;
  while(serverIPAddr_port[i] != ':') i++;
  int l = strlen(serverIPAddr_port);

  /* server IP addr */
  char serverIPAddr[i];
  /* server PORT */
  char serverPORT[l-i];

  /* Extracting serverIP address */
  for(int x=0;x<i;x++) serverIPAddr[x] = serverIPAddr_port[x];
  serverIPAddr[i] = '\0';
    
  /* Extracting serverPORT */
  for(int x=i+1;x<=l;x++) serverPORT[x-i-1] = serverIPAddr_port[x];
  serverPORT[l-i] = '\0';

  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) { perror("socket failed"); exit(20); }

  dest_addr.sin_family = AF_INET; // host byte order
  dest_addr.sin_port = htons(atoi(serverPORT)); // network byte order
  dest_addr.sin_addr.s_addr = inet_addr(serverIPAddr);
  memset(&(dest_addr.sin_zero), '\0', 8); // set to zero the rest of the struct 

  /* Establishing Connection with server */
  connect_check = connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
  if(connect_check < 0) {perror("connection with server failed"); exit(2);}
  else {cout<<"ConnectDone: "<<serverIPAddr_port<<"\n";}

  if((opt[0] == 'g') &&
     (opt[1] == 'e') &&
     (opt[2] == 't'))
  {  
    int filesize = 0;

    sendname = send(sockfd,filename,strlen(filename)+1,0);
    if(sendname < 0) {perror("unable to send get file message"); exit(21);}

    FILE* f = fopen(argv[3],"w");

    if(f)
    {
      memset(&receive_buffer,'\0',MAXSIZE);
      recv_check = recv(sockfd, receive_buffer, MAXSIZE, 0);
      if(recv_check == 0) {cout<<"Connection with server closed\n";}
      fwrite(receive_buffer,1,recv_check,f);
      filesize += recv_check;
      usleep(1000*atoi(argv[4]));

      while(recv_check)
      {
        recv_check = recv( sockfd , receive_buffer, MAXSIZE, 0);
        if(recv_check != 0)
        {
          fwrite(receive_buffer,1,recv_check,f);
          filesize += recv_check;
        }
        usleep(1000*atoi(argv[4]));
      }

    }
    else
    {
      perror("Unable to open file"); exit(3);
    }

    fclose(f);
    cout<<"FileWritten: "<<filesize<<" bytes\n";
    close(sockfd);

  }
  else if((opt[0] == 'p') &&
          (opt[1] == 'u') &&
          (opt[2] == 't'))
  {
    long long int filesize = 0;
    long long int total_bytes_sent = 0;

    sendname = send(sockfd,filename,strlen(filename)+1,0);
    if(sendname < 0) {perror("Unable to send put file message"); exit(23);}

    ifstream File(argv[3]);

    if(!File.good()) 
      { 
          cout<<"FileTransferFail\n";
          fprintf(stderr,"%s","File not present or not readable");
          close(sockfd);
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
        memset(&send_buffer,'\0',MAXSIZE);
        File.read(send_buffer,num_bytes_to_send);

        bytes_sent = send(sockfd,&send_buffer,num_bytes_to_send,0);
        //usleep(1000*atoi(argv[4]));
      
        total_bytes_sent += bytes_sent;
        length -= bytes_sent;
      }

      cout<<"TransferDone: "<<total_bytes_sent<<" bytes"<<"\n";
      close(sockfd);
    }
  }

  return 0;
}