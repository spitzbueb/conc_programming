CFLAGS = -g

all: Server Client

clean: 
	rm run test

Server: Server.c
	gcc $(CFLAGS) -o run Server.c 

Client: Client.c
	gcc -o test Client.c
