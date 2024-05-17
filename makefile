CC = gcc

CFLAGS = -Wall -g

all: jobCommander jobExecutorServer

jobCommander.o: jobCommander.c
	$(CC) $(CFLAGS) -c $<

jobExecutorServer.o: jobExecutorServer.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o jobCommander jobExecutorServer

run: jobCommander
	./jobCommander