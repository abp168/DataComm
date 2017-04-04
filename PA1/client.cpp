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
#include <fstream>
#include <string>

struct hostent *s;
struct sockaddr_in server;

int main(int argc, char *argv[])
{
	int mysocket = 0;
	socklen_t slen = sizeof(server);
	char r_port[1024];
	const char *n_msg = "123";
	char str[512];
	int n_port = atoi(argv[2]);
	s = gethostbyname(argv[1]);
	memset((char *) &server, 0, sizeof(server)); //clears memory of server 
	server.sin_family = AF_INET; //IPV4
	server.sin_port = htons(n_port); //random port number for data exchange
	bcopy((char *)s->h_addr,(char *) &server.sin_addr.s_addr,s->h_length); //copies byte sequence  length of host address to server address
        if((mysocket=socket(AF_INET,SOCK_STREAM,0))== -1) //error check for creation of TCP socket
	  {
	    fprintf(stderr,"Socket descriptor error");
	  }
	printf("Hostname: %s\n",s->h_name);  
	connect(mysocket,(struct sockaddr *) &server,slen); //connects socket to server
	inet_ntop(AF_INET,&(server.sin_addr),str,sizeof(str)); //gets ip address of server
	printf("Connecting to %s\n",str);
	send(mysocket,n_msg,strlen(n_msg),0); //send negotation message
	if((recv(mysocket,r_port,sizeof(r_port),0)) == -1) //error check for receive from server
	  {
	    fprintf(stderr,"Receive error. Error code: %d\n",errno);
	  }
	  
	printf("Random Port: %s\n",r_port);
	close(mysocket);
	/* Stage 2: Transaction using UDP sockets below */
	int udp_socket = 0;
	int int_r_port;
	int_r_port = atoi(r_port);
	server.sin_port = htons(int_r_port); //change server port to mathc random port number sent from server
	bcopy((char *)s->h_addr,(char *) &server.sin_addr.s_addr,s->h_length); //copies byte sequence  length of host address to server address
	if((udp_socket=socket(AF_INET,SOCK_DGRAM,0)) == -1) //error check for creation of udp socket
	  {
	    fprintf(stderr,"Socket descriptor error");
	  }
	connect(udp_socket,(struct sockaddr *) &server,slen); //connect socket to server
	inet_ntop(AF_INET,&(server.sin_addr),str,sizeof(str));
	std::ifstream readFile;
	readFile.open(argv[3]); //open file given in command
	char payload[5];
	char eof[]= {'1'};
	char rec_payload[5]={};
	int num_bytes =0;
	memset(payload,0,sizeof(payload));
	memset(payload,0,sizeof(rec_payload));
	if(readFile.is_open())
	  {
	    for(int i =0 ;readFile.read(payload,4); i++) // while file is readable (no eof)
	      {
		sendto(udp_socket,payload,4,0,(struct sockaddr*)&server,sizeof(server));
		memset(payload,0,sizeof(payload));
		memset(payload,0,sizeof(rec_payload));
		recvfrom(udp_socket,rec_payload,sizeof(rec_payload),0,(struct sockaddr *) &server, &slen);
		 printf("%s\n",rec_payload);
		
		
	      }
	    /* This segment of code is to send, receive, and print out the last character of the file before eof flag */
	    readFile.read(payload,4);  
	    sendto(udp_socket,payload,4,0,(struct sockaddr*)&server,sizeof(server));
	    recvfrom(udp_socket,rec_payload,sizeof(rec_payload),0,(struct sockaddr *) &server, &slen);
	    printf("%s\n",rec_payload);
	    /*********************************************************************************************************/
	    if(readFile.eof()) 
	      {
		sendto(udp_socket,eof,1,0,(struct sockaddr*)&server,sizeof(server)); // sends flag to server that the end of file has been reached
	      }

	close(udp_socket);
	return 0;
	
	  }
}
