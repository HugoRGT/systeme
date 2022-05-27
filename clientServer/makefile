CC = gcc -Wall

all:server client test_cli test_serv
	$(export LD_LIBRARY_PATH=$(pwd))
	rm -f *.o
server: server.o libmessage.so
	$(CC) -L${PWD} server.o -lmessage -o server

client: client.o libmessage.so
	$(CC) -L${PWD} client.o -lmessage -o client

test_serv: test_serv.o
	$(CC) -L${PWD} test_serv.o -lmessage -o test_serv


test_cli: test_cli.o
	$(CC) -L${PWD} test_cli.o -lmessage -o test_cli


server.o: server.c
	$(CC) -D__EXTENSIONS__ server.c  -c -o server.o

client.o: server.c
	$(CC) -D__EXTENSIONS__ client.c  -c -o client.o

test_serv.o: server.c
	$(CC) -D__EXTENSIONS__ test_serv.c  -c -o test_serv.o

test_cli.o: test_cli.c
	$(CC) -D__EXTENSIONS__ test_cli.c  -c -o test_cli.o





message.o :message.c message.h
	$(CC) message.c -fPIC -c -o message.o


libmessage.so: message.o
	$(CC) -shared message.o -o libmessage.so

clean:
	rm -f *.o
mrproper: clean
	rm -f server
	rm -f libmessage.so
	rm -f client
