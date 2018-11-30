all: server client

server : server.o
	gcc -o server server.o

client : client.o
	gcc -o client client.o

server.o : server.c
	gcc -c -o server.o server.c

client.o : client.c
	gcc -c -o client.o client.c

clean	:
	rm *.o 
