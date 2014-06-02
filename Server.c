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
	char name[256];
	int size;
	char content[4096];
	short semval[1];
};

key_t shmkey, semkey;
int shmid, shmid_cleanup;
int semid, semid_cleanup;
int sockfd, newsockfd;
struct datei *dateien;

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

int create_shm(key_t key, const char *txt, const char *etxt)
{
	int result = shmget(key, 1048576, IPC_CREAT | IPC_EXCL | 0600);
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
	shmid = create_shm(shmkey, "create","SHMGET FAILED!\n");
	shmid_cleanup = shmid;
	
	dateien = (struct datei *) shmat(shmid,NULL,0);
/* Ende Aufbau Shared-Memory */

/* Aufbau Semaphore */
	semkey = ftok(REF_FILE,1);
	if(semkey < 0)
	{
		perror("ERROR FTOK SEMAPHOR!\n");
		exit(1);
	}
	
	printf("Setting up Semaphor!\n");
	semid = create_sem(semkey, 10, "create", "SEMAPHOR FAILED!\n", IPC_CREAT);
	semid_cleanup = semid;
	
	struct sembuf sem_one, sem_all, sem_one_undo, sem_all_undo;
	
	sem_one.sem_num = 0;
	sem_one.sem_op = -1;
	sem_one.sem_flg = SEM_UNDO;
	
	sem_one_undo.sem_num = 0;
	sem_one_undo.sem_op = 1;
	sem_one_undo.sem_flg = SEM_UNDO;
	
	sem_all.sem_num = 0;
	sem_all.sem_op = -10;
	sem_all.sem_flg = SEM_UNDO;
	
	sem_all_undo.sem_num = 0;
	sem_all_undo.sem_op = 10;
	sem_all_undo.sem_flg = SEM_UNDO;
	
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
					int zaehler = 0;
					char *temp;
					char message[5000];
					temp = (char*)malloc(sizeof(dateien));
									
					while((int) strlen(dateien[counter].name) != 0)
					{
						if(strcmp(dateien[counter].name,"EMPTY") == 0)
						{
							counter++;
						}
						else
						{
							strcat(temp,dateien[counter].name);
							strcat(temp,"\n");
							zaehler++;
							counter++;
						}
					}
					
					sprintf(message,"ACK %d\n%s",zaehler,temp);	
										
					retcode = write(newsockfd,message,strlen(message));
					
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
					
					temp[0] = '\0';
					message[0] = '\0';
				}
				else if(strcmp(befehl,"CREATE") == 0)
				{
					int counter = 0;
					int status = 0;
					char *message;
										
					while((int) strlen(dateien[counter].name) != 0)
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
						
						counter=0;
						while((int) strlen(dateien[counter].name) != 0)
						{
							if(strcmp(dateien[counter].name,"EMPTY") == 0)
							{
								break;
							}
							counter++;
						}
						
						dateien[counter].semval[0] = (short) 10;
						
						strcpy(dateien[counter].name,dateiname);
						dateien[counter].size = atoi(groesse);
						retcode = write(newsockfd,"CONTENT:\n",10);
						retcode = read(newsockfd,buffer,255);
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
					
					while((int) strlen(dateien[counter].name) != 0)
					{
						if(strcmp(dateien[counter].name,dateiname) == 0)
						{
							
							if(semctl(semid,0,SETALL,&dateien[counter].semval[0]) < 0)
							{
								printf("ERROR SEMCTL FOR FILE\n");
							}
							
							if(semop(semid,&sem_one,1) < 0)
							{
								printf("ERROR SEMOP FOR FILE\n");
							}
							
							dateien[counter].semval[0] = semctl(semid,0,GETVAL,0);
							
							sprintf(message,"FILECONTENT %s %d\n%s\n",dateien[counter].name,dateien[counter].size,dateien[counter].content);
														
							if(semop(semid,&sem_one_undo,1) < 0)
							{
								printf("ERROR SEMOP_UNDO FOR FILE\n");
							}
							
							dateien[counter].semval[0] = semctl(semid,0,GETVAL,0);
							
							break;
						}
						else
						{
							strcpy(message,"NOSUCHFILE\n");
						}
						counter++;
					}
					
					
					retcode = write(newsockfd,message,strlen(message));
					message[0] = '\0';
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
					
				}
				else if(strcmp(befehl,"UPDATE") == 0)
				{
					int counter = 0;
					char *message;
					
					while((int) strlen(dateien[counter].name) != 0)
					{
						if(strcmp(dateien[counter].name,dateiname) == 0)
						{
							if(semctl(semid,0,SETALL,&dateien[counter].semval[0]) < 0)
							{
								printf("ERROR SEMCTL FOR FILE\n");
							}
							dateien[counter].size = atoi(groesse);
							strcpy(dateien[counter].content,"");
							message = "CONTENT:\n";
							
							if(semop(semid,&sem_all,1) < 0)
							{
								printf("ERROR SEMOP FOR FILE\n");
							}
							
							dateien[counter].semval[0] = semctl(semid,0,GETVAL,0);
							
							break;
						}
						else
						{
							message = "NOSUCHFILE\n";
						}
						
						counter++;
					}
					
					retcode = write(newsockfd,message,strlen(message));
										
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
					
					if(strcmp(message,"NOSUCHFILE\n") != 0)
					{
						retcode = read(newsockfd,buffer,256);
					
						if(retcode < 0)
						{
							perror("ERROR READING SOCKET!\n");
							exit(1);
						}
					
						strcpy(dateien[counter].content,buffer);
						
						retcode = write(newsockfd,"UPDATED\n",9);
										
						if(retcode < 0)
						{
							perror("ERROR WRITING SOCKET!\n");
							exit(1);
						}
						
						if(semop(semid,&sem_all_undo,1) < 0)
						{
							printf("ERROR SEMOP FOR FILE\n");
						}
					
						dateien[counter].semval[0] = semctl(semid,0,GETVAL,0);
					}
				}
				else if(strcmp(befehl,"DELETE") == 0)
				{
					int counter = 0;
					char *message;
					
					while((int) strlen(dateien[counter].name) != 0)
					{
						if(strcmp(dateien[counter].name,dateiname) == 0)
						{
							if(semctl(semid,0,SETALL,&dateien[counter].semval[0]) < 0)
							{
								printf("ERROR SEMCTL FOR FILE\n");
							}
							
							if(semop(semid,&sem_all,1) < 0)
							{
								printf("ERROR SEMOP FOR FILE\n");
							}
					
							dateien[counter].semval[0] = semctl(semid,0,GETVAL,0);
							strcpy(dateien[counter].name,"EMPTY");
							strcpy(dateien[counter].content,"");
							dateien[counter].size = 0;
							message = "DELETED\n";
							
							if(semop(semid,&sem_all_undo,1) < 0)
							{
								printf("ERROR SEMOP FOR FILE\n");
							}
					
							dateien[counter].semval[0] = semctl(semid,0,GETVAL,0);
							
							break;
						}
						else
						{
							message = "NOSUCHFILE\n";
						}
						
						counter++;
					}
					
					write(newsockfd,message,strlen(message));
					if(retcode < 0)
					{
						perror("ERROR WRITING SOCKET!\n");
						exit(1);
					}
				}
				else if(strcmp(befehl,"LISTALL") == 0)
				{
					int counter = 0;
					
					while((int) strlen(dateien[counter].name) != 0)
					{
						printf("%s\n",dateien[counter].name);
						counter++;
					}
					
					write(newsockfd,"OK\n",3);
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