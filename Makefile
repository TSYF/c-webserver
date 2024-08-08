CC=gcc

TestServer: Networking/Server.c Networking/TestServer.c 
	$(CC) Networking/Server.c \
			Networking/TestServer.c \
		-o TestServer