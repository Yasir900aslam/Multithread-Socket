all: Server Client

Server: Server.c
	gcc Server.c -lpthread -o Server

Client: Client.c
	gcc -o Client Client.c

clean:
	rm -f Client Server

