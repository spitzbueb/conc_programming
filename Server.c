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

key_t key;
int shmid;


int main(int argc, char *argv[])
{
	
	int sockfd, clntLen, portno, newsockfd;
	char buffer[256];
	struct sockaddr_in server_addr, clnt_addr;
	int retcode;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0)
	{
		perror("ERROR OPENING SOCKET!\n");
		exit(1);
	}
	
	bzero((char *) &server_addr, sizeof(server_addr));
	portno = 6666;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);
	
	if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		perror("ERROR BINDING!\n");
		exit(1);
	}
	
	listen(sockfd,100);
	clntLen = sizeof(clnt_addr);
	
	while(1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &clnt_addr, &clntLen);
		
		int pid;
		
		pid = fork();
		
		if(pid < 0)
		{
			close(newsockfd);
			perror("ERROR ACCEPTING CONNECTION!\n");
			exit(1);
		}
		else if(pid > 0)
		{
			continue;
		}
		else
		{
			/* CHILD PART */
			while(1)
			{
				retcode = read(newsockfd, buffer, 255);
				if(retcode < 0)
				{
					perror("ERROR READING SOCKET!\n");
					exit(1);
				}
			
				printf("Your Message: %s\n", buffer);
			
				retcode = write(newsockfd, buffer, strlen(buffer));
				if(retcode < 0)
				{
					printf("ERROR WRITING SOCKET!\n");
					exit(1);
				}
				
				printf("DONE!\n");
			}
		}
	}
}