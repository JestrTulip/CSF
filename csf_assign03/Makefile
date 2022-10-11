# Sample Makefile

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra

# Links together files needed to create executable
main: main.o cache_functions.o
	$(CC) -o main main.o functions.o

# Compiles mainFile.c to create mainFile.o
# Note that we list functions.h here as a file that
# main_file.c depends on, since main_file.c #includes it
main.o: main.c cache_functions.h
	$(CC) $(CFLAGS) -c main.c

# Compiles functions.c to create functions.o
# Note that we list functions.h here as a file that
# functions.c depends on, since functions.c #includes it
cache_functions.o: cache_functions.c cache_functions.h
	$(CC) $(CFLAGS) -c cache_functions.c

# Removes all object files and the executable named main,
# so we can start fresh
clean:
	rm -f *.o main
