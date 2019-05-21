CC = gcc
OPTIONS = -o
all: main.o myshell.o utility.o
	$(CC) -I . main.o myshell.o utility.o $(OPTIONS) myshell
main.o: main.c myshell.h utility.h
	$(CC) -I . -c main.c
myshell.o: myshell.c myshell.h utility.h
	$(CC) -I . -c myshell.c
utility.o: utility.c utility.h
	$(CC) -I . -c utility.c
clean:
	rm -rf *.o