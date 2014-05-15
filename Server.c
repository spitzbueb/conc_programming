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
#include <semaphore.h>

struct datei
{
	char *name;
	int size;
	char *content;
	short semval[1];
};

key_t shmkey, semkey;
int shmid, shmid_cleanup;
int semid, semid_cleanup;
int sockfd, newsockfd;
int x = 0;

const char *REF_FILE = "./shm_sem_ref.dat";

void close_socket()
{
	close(sockfd);
	close(newsockfd);
}

void cleanup()
{
	if (shmid_cleanup > 0)
	{
		int retcode = shmctl(shmid_cleanup, IPC_RMID, NULL);
		shmid_cleanup = 0;
		
		if(retcode < 0)
		{
			printf("Error cleaning Shared-Memory!\n");
		}
	}
	
	if (semid_cleanup > 0)
	{
		int retcode = semctl(semid_cleanup, 0, IPC_RMID, NULL);
		semid_cleanup = 0;
		
		if(retcode < 0)
		{
			printf("Error cleaning Semaphore!\n");
		}
	}
}

int create_shm(key_t key, const char *txt, const char *etxt, int flags)
{
	int result = shmget(key, 256, flags | 0600);
	if (result < 0)
	{
		printf("%s", etxt);
		exit(1);
	}
	return result;
}

int create_sem(key_t key, const int sem_size, const char *txt, const char *etxt, int flags)
{
	int result = semget(key, sem_size, flags | 0600);
	if (result < 0)
	{
		printf("%s", etxt);
		exit(1);
	}
	printf("SemID=%d, key=%ld\n", result, (long) key);
	return result;
}

void sig_handler(int sig)
{
	cleanup();
	close_socket();
	exit(0);
}

int main(int argc, char *argv[])
{
	int retcode;
	
	
/* SIGINT abfangen */
	signal(SIGINT, sig_handler);
	
/* Aufbau Shared-Memory */
	FILE *fptr;
	fptr = fopen(REF_FILE,"rb+");
	if(fptr == NULL)
	{
		printf("CREATING REF_FILE!\n");
		fptr = fopen(REF_FILE, "wb");
	}
	
	shmkey = ftok(REF_FILE,1);
	if(shmkey < 0)
	{
		perror("ERROR FTOK SHM!\n");
		exit(1);
	}
	
	printf("Setting up SHM!\n");
	shmid = create_shm(shmkey, "create","SHMGET FAILED!\n",IPC_CREAT);
	shmid_cleanup = shmid;
	
	struct datei *dateien = (struct datei *) shmat(shmid,NULL,0);
/* Ende Aufbau Shared-Memory */

/* Aufbau Semaphore */
	semkey = ftok(REF_FILE,1);
	if(semkey < 0)
	{
		perror("ERROR FTOK SEMAPHOR!\n");
		exit(1);
	}
	
	printf("Setting up Semaphor!\n");
	semid = create_sem(semkey, 1, "create", "SEMAPHOR FAILED!\n", IPC_CREAT);
	semid_cleanup = semid;
	short semval[1];
	semval[0] = (short) 10;
	
	struct sembuf sem_one, sem_all;
	
	sem_one.sem_num = 0;
	sem_one.sem_op = -1;
	sem_one.sem_flg = SEM_UNDO;
	
	sem_all.sem_num = 0;
	sem_all.sem_op = -10;
	sem_all.sem_flg = SEM_UNDO;
	
/* Ende Aufbau Semaphor */

/* Aufbau Netzwek-Socket */	
	int clntLen, portno;
	char buffer[256];
	struct sockaddr_in server_addr, clnt_addr;
	
	
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
/* Ende Aufbau Netzwerk-Socket */
	
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
				
				int zaehler = 0;
				retcode = read(newsockfd, buffer, 255);
				if(retcode < 0)
				{
					perror("ERROR READING SOCKET!\n");
					exit(1);
				}
				
				char delimiter[] = " ,;:\n";
				
				char *befehl = strtok(buffer,delimiter);
				char *dateiname = strtok(NULL,delimiter);
				char *groesse = strtok(NULL,delimiter);
				
				if(strcmp(befehl,"LIST") == 0)
				{
					int counter = 0;
					
					while(dateien[counter].name != NULL)
					{
						printf("%d\n",counter);
						counter++;
					}
					
	
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
					
					
				}
				else if(strcmp(befehl,"CREATE") == 0)
				{
					int counter = 0;
					int status = 0;
					char *message;
					
					while(dateien[counter].name != NULL)
					{
						if(strcmp(dateien[counter].name,dateiname) != 0)
						{
							counter++;
						}
						else
						{
							status = 1;
							break;
						}
					}
					
					if(status == 1)
					{
						message = "FILEEXISTS\n";
					}
					else
					{
						dateien[counter].name = (char*)calloc(strlen(dateiname),sizeof(char));
						strcpy(dateien[counter].name,dateiname);
						dateien[counter].size = atoi(groesse);
						dateien[counter].semval[0] = (short) 10;
						retcode = write(newsockfd,"CONTENT:\n",10);
						retcode = read(newsockfd,buffer,255);
						dateien[counter].content = (char*)calloc(dateien[counter].size,sizeof(char));
						strcpy(dateien[counter].content,buffer);
						message = "FILECREATED\n";
					}
					
					retcode = write(newsockfd,message,strlen(message));
					
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
					
				}
				else if(strcmp(befehl,"READ") == 0)
				{
					int counter=0;
					char message[256];
												
					printf("vor schlaufe\n");
					while(dateien[counter].name != NULL)
					{
						if(strcmp(dateien[counter].name,dateiname) == 0)
						{
							sprintf(message,"FILECONTENT %s %d\n%s\n",dateien[counter].name,dateien[counter].size,dateien[counter].content);
							printf("Nach sprintf\n");
							break;
						}
						else
						{
							strcpy(message,"NOSUCHFILE\n");
						}
						counter++;
					}
					
					
					retcode = write(newsockfd,message,strlen(message));
					printf("Done\n");
					message[0] = '\0';
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
				}
				else if(strcmp(befehl,"UPDATE") == 0)
				{
					retcode = write(newsockfd,"OK\n",3);
					int counter = 0;
					while(strcmp(dateiname,dateien[counter].name) != 0)
					{
						counter++;
					}
					
					retcode = read(newsockfd,buffer,255);
					if(retcode < 0)
					{
						perror("ERROR READING SOCKET!\n");
						exit(1);
					}
					dateien[counter].content = (char*)calloc(strlen(buffer),sizeof(char));
					strcpy(dateien[counter].content,buffer);
					retcode = write(newsockfd,"Gespeichert!\n",14);
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
				}
				else if(strcmp(befehl,"DELETE") == 0)
				{
					int counter = 0;
					
					while(strcmp(dateiname,dateien[counter].name) != 0)
					{
						counter++;
					}
					
					strcpy(dateien[counter].name,"");
					strcpy(dateien[counter].content,"");
					
					retcode = write(newsockfd,"Datei gelöscht!\n",17);
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
				}
				else
				{
					retcode = write(newsockfd,"Action not known!\n",19);
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
				}			
				
			}
		}
	}
}