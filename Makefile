CC=gcc
CFLAGS=-fPIC -O2

test:
	$(CC) src/Networking/TcpServer/TcpServer.c \
			src/Networking/TcpServer/TestTcpServer.c \
		-o TestTcpServer

testHttp:
	$(CC) src/Networking/TcpServer/TcpServer.c \
			src/Networking/HttpServer/HttpServer.c \
			src/Utilities/HashTable/HashTable.c \
			src/Networking/HttpServer/TestHttpServer.c \
		-o TestHttpServer

debugHttp:
	$(CC) -ggdb \
	-fdiagnostics-color=always \
		-g src/Networking/TcpServer/TcpServer.c \
		src/Utilities/HashTable/HashTable.c \
		src/Networking/HttpServer/HttpServer.c \
		src/Networking/HttpServer/TestHttpServer.c \
	-o DebugHttpServer