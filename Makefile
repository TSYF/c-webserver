CC=gcc

TestServer:	DataStructures/Node.c DataStructures/LinkedList.c Networking/Server.c Networking/TestServer.c 
	$(CC) DataStructures/Node.c \
			DataStructures/LinkedList.c \
			Networking/Server.c \
			Networking/TestServer.c \
		-o TestServer