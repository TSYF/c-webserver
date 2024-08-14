CC=gcc
CFLAGS=-fPIC -O2

test: Networking/BasicServer.c Networking/TestServer.c 
	$(CC) Networking/BasicServer.c \
			Networking/TestServer.c \
		-o TestServer
	./TestServer

test.bkp: Networking.bkp/BasicServer.c Networking.bkp/TestServer.c  
	$(CC) Networking.bkp/BasicServer.c \
			Networking/TestServer.c \
		-o TestServer-bkp
	./TestServer-bkp

lib: Networking/BasicServer.o
	$(CC) -shared -o libBasicServer.so Networking/BasicServer.o

Networking/BasicServer.o: Networking/BasicServer.c Networking/BasicServer.h
	$(CC) $(CFLAGS) -c Networking/BasicServer.c -o Networking/BasicServer.o

install: lib
	cp libBasicServer.so $(DESTDIR)/usr/lib/libBasicServer.so
	cp Networking/BasicServer.h $(DESTDIR)/usr/include/BasicServer.h

uninstall:
	rm -f $(DESTDIR)/usr/lib/libBasicServer.so
	rm -f $(DESTDIR)/usr/include/BasicServer.h

clean:
	rm -f Networking/BasicServer.o libBasicServer.so

all: lib install