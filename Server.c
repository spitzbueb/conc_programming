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

struct datei
{
	char *name;
	int size;
	char *content;
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
	dateien[0].name = "ENDE";
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
	retcode = semctl(semid, 1, SETALL, &semval);
	
	if (retcode < 0)
	{
		perror("ERROR CHANGING SEMAPHORE!\n");
		cleanup();
		exit(1);
	}
	
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
					char *message = "Dateien: ";
					int counter = 0;
					
					while(strcmp(dateien[counter].name,"ENDE")!=0)
					{
						printf("%s\n",dateien[counter].name);
						counter++;
					}
					
					retcode = write(newsockfd,message,strlen(message));
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
					
					
				}
				else if(strcmp(befehl,"CREATE") == 0)
				{
					int counter = 0;
					
					while(dateien[counter].name != NULL)
					{
						if(strcmp(dateien[counter].name,"ENDE") == 0)
						{
							printf("Ende gefunden!\n");
							dateien[counter].name = NULL;
							dateien[counter+1].name = "ENDE";
						}
						else
						{
							counter++;
						}
					}
					
					dateien[counter].name = dateiname;
					dateien[counter].size = atoi(groesse);
					
					retcode = write(newsockfd,"Datei wurde erstellt\n",22);
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
					
				}
				else if(strcmp(befehl,"READ") == 0)
				{
					int counter=0;
					
					while(strcmp(dateiname,dateien[counter].name) != 0)
					{
						counter++;
					}
					
					retcode = write(newsockfd,dateien[counter].content,sizeof(dateien[counter].content));
					
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
				}
				else if(strcmp(befehl,"UPDATE") == 0)
				{
					int counter = 0;
					while(strcmp(dateiname,dateien[counter].name) != 0)
					{
						counter++;
					}
					
					retcode = write(newsockfd,"Bitte neuen Inhalt eingeben!\n",30);
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
					retcode = read(newsockfd,buffer,255);
					if(retcode < 0)
					{
						perror("ERROR READING SOCKET!\n");
						exit(1);
					}
					dateien[counter].content = buffer;
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
					
					dateien[counter].name = NULL;
					dateien[counter].size = 0;
					dateien[counter].content = NULL;
					
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