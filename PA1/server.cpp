#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <fstream>

;
int main(int argc, char *argv[])
{
  
  struct sockaddr_in server,client;
  char str[512];
  int mysocket = 0;
  int newfd = 0;
  char n_msg[1024]={};
  int n_port = atoi(argv[1]);
  const char *r_port ="15250";
  int bytes_sent;
  socklen_t clen = sizeof(client);
  if(( mysocket= socket(AF_INET,SOCK_STREAM,0))==-1) //error check for TCP socket creation
    {
      fprintf(stderr,"Socket descriptor error");
    }
  memset((char *)&server, 0, sizeof(server));
  server.sin_family = AF_INET; //IPV4
  server.sin_port = htons(n_port); //negotiation port
  server.sin_addr.s_addr = htonl(INADDR_ANY); 
  if((bind(mysocket,(struct sockaddr *)&server,sizeof(server))) == -1) //error check for binding socket to server
    {
      printf("Cannot bind. Error code: %d\n",errno);
    }
  listen(mysocket,10); //listens to client for 10 sec
  newfd = accept(mysocket, (struct sockaddr *) &client, &clen); //creates new socket newfd after accepting mysocket

  inet_ntop(AF_INET,&(client.sin_addr),str,sizeof(str));
  if((recv(newfd,n_msg,sizeof(n_msg),0)) == -1) //error check for receive from client
    {
      fprintf(stderr,"Receive error. Error code: %d\n",errno);
    }
  else
    {
      printf("%s\nNegotiation detected. Selected the follwoing random port: %s\n",n_msg,r_port);
      bytes_sent =send(newfd,r_port,strlen(r_port),0);
    }

  close(mysocket);
  close(newfd);

/* Stage 2: Transcation using UDP sockes below */
int udp_socket = 0;
char payload[5];
std::ifstream readFile;
 std::ofstream outfile;
int int_r_port = atoi(r_port);
memset(payload,0,sizeof(payload));
 server.sin_port = htons(int_r_port);
 if((udp_socket=socket(AF_INET,SOCK_DGRAM,0)) == -1) //error check for UDP socket creation
  {
    fprintf(stderr,"Socket descriptor error");
  }
 if((bind(udp_socket,(struct sockaddr *)&server,sizeof(server))) == -1) //error check for bind to server
  {
    printf("Cannot bind. Erro code: %d\n",errno);
    }
 outfile.open("output.txt"); //creates output.txt file
 for(int k = 0;recvfrom(udp_socket,payload,sizeof(payload),0,(struct sockaddr *)&client, &clen); k++) //while server is receiving from client
   {
     if(payload[0] == '1') 
       {
	 break;  //eof flag sent from client break loop 
       }
     outfile.write(payload,sizeof(payload)); 
     for(int i =0; i<sizeof(payload); i++) //capatilize payloads as they are received
      {
	payload[i]=toupper(payload[i]);
      }
     sendto(udp_socket,payload,4,0,(struct sockaddr *)&client, clen); //send capatilized payloads back to client
      memset(payload,0,sizeof(payload));
   }
 outfile.close();
   
 close(udp_socket);
}
