CC=gcc
CFLAGS=-fPIC -O2

test: src/Networking/TcpServer
	$(CC) src/Networking/TcpServer/TcpServer.c \
			src/Networking/TcpServer/TestTcpServer.c \
		-o TestTcpServer