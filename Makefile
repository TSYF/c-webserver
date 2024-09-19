CC=gcc
CFLAGS=-fPIC -O2

test:
	$(CC) core/TcpServer/TcpServer.c \
			core/TcpServer/TestTcpServer.c \
		-o TestTcpServer

testHttp:
	$(CC) core/TcpServer/TcpServer.c \
			core/HttpServer/HttpServer.c \
			utils/HashTable/HashTable.c \
			core/HttpServer/TestHttpServer.c \
		-o TestHttpServer

debugHttp:
	$(CC) -ggdb \
	-fdiagnostics-color=always \
		-g core/TcpServer/TcpServer.c \
		utils/HashTable/HashTable.c \
		core/HttpServer/HttpServer.c \
		core/HttpServer/TestHttpServer.c \
	-o DebugHttpServer