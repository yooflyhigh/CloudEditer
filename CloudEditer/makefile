all: server client

server : server.o
	g++ -o server server.o

client : client.o
	g++ -o client client.o

server.o : server.cc
	g++ -c -o server.o server.cc

client.o : client.cc
	g++ -c -o client.o client.cc

clean	:
	rm *.o 
