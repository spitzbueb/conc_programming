#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char *servIP;
	
	char buffer[256];
	
	portno = 6666;
	servIP="127.0.0.1";
	
	sockfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sockfd < 0)
	{
		printf("ERROR CREATING SOCKET!\n");
		exit(1);
	}
	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(servIP);
	serv_addr.sin_port = htons(portno);
	
	n = connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if(n<0)
	{
		printf("ERROR CONNECTING!\n");
		exit(1);
	}
	
	while(1)
	{
		fgets(buffer,256,stdin);
		
		n = write(sockfd, buffer, strlen(buffer));
		
		if(n<0)
		{
			printf("ERROR WRITING TO SOCKET!\n");
			exit(1);
		}
		
		bzero(buffer,256);
		
		n = read(sockfd, buffer, 255);
		if(n<0)
		{
			printf("ERROR READING SOCKET!\n");
			exit(1);
		}
		
		printf("Message was: %s\n", buffer);
	}
	
	
}
