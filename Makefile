CC=g++
CFLAGS= 
LDFLAGS= 
DEFINES= 

.PHONY: all 

all: client server clean

client: client.o  
	$(CC) client.o -o client

client.o: client.cpp 
	$(CC) -c client.cpp

server: server.o  
	$(CC) server.o -o server

server.o: server.cpp 
	$(CC) -c server.cpp

clean:
	rm *.o

