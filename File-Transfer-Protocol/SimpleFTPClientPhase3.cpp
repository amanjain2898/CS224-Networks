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

using namespace std;

#define MAXSIZE 1000

/* Print usage of program */
void usage(char *progname) {
  fprintf(stderr, "%s <serverIPAddr:port> <fileToReceive> <receiveInternal>\n",
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
  char buffer[MAXSIZE];
  /* File to receive */
  int connect_check;
  int recv_check;
  int sendname;
  int filesize = 0;
  
  /* Get command-line arguments */
  if(argc != 4) { usage(argv[0]); exit(1); }
  serverIPAddr_port = argv[1];
  int len = strlen(argv[2]);
  char fileToReceive[len+4];
  fileToReceive[0] = 'g';
  fileToReceive[1] = 'e';
  fileToReceive[2] = 't';
  fileToReceive[3] = ' ';
  for(int i=4;i<len+4;i++)
  {
    fileToReceive[i] = argv[2][i-4];
  }
  fileToReceive[len+4] = '\0';

  //cout<<fileToReceive<<endl;

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

  sendname = send(sockfd,fileToReceive,strlen(fileToReceive),0);
  if(sendname < 0) {perror("unable to send get file message"); exit(21);}

  FILE* f = fopen(argv[2],"w");

  if(f)
  {
    recv_check = recv(sockfd, buffer, MAXSIZE, 0);
    if(recv_check == 0) {cout<<"Connection with server closed\n";}
    fwrite(buffer,1,recv_check,f);
    filesize += recv_check;
    usleep(1000*atoi(argv[3]));

    while(recv_check)
    {
      recv_check = recv( sockfd , buffer, MAXSIZE, 0);
      if(recv_check != 0)
      {
        fwrite(buffer,1,recv_check,f);
        filesize += recv_check;
      }
      usleep(1000*atoi(argv[3]));
    }

  }
  else
  {
    perror("Unable to open file"); exit(3);
  }

  fclose(f);
  cout<<"FileWritten: "<<filesize<<" bytes\n";
  close(sockfd);

  return 0;
}