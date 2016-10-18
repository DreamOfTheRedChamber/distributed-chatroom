CC=gcc
LD=gcc
CFLAGS=-g -Wall 
CPPFLAGS=-I. -I/home/cs437/exercises/ex3/include
SP_LIBRARY_DIR=/home/cs437/exercises/ex3

all: client server

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

client:  $(SP_LIBRARY_DIR)/libspread-core.a client.o linkListFunc.o
	$(LD) -o $@ client.o linkListFunc.o $(SP_LIBRARY_DIR)/libspread-core.a -ldl -lm -lrt -lnsl $(SP_LIBRARY_DIR)/libspread-util.a

server:  $(SP_LIBRARY_DIR)/libspread-core.a server.o linkListFunc.o
	$(LD) -o $@ server.o linkListFunc.o $(SP_LIBRARY_DIR)/libspread-core.a -ldl -lm -lrt -lnsl $(SP_LIBRARY_DIR)/libspread-util.a

clean:
	rm -f *.o client server

