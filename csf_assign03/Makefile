# Sample Makefile

CC = g++
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)

# Links together files needed to create executable
csim: csim.o cache_functions.o
	$(CC) -o main main.o functions.o

# Compiles mainFile.c to create mainFile.o
# Note that we list functions.h here as a file that
# main_file.c depends on, since main_file.c #includes it
csim.o: main.cpp cache_functions.h
	$(CC) $(CFLAGS) -c main.cpp

# Compiles functions.c to create functions.o
# Note that we list functions.h here as a file that
# functions.c depends on, since functions.c #includes it
cache_functions.o: cache_functions.cpp cache_functions.h
	$(CC) $(CFLAGS) -c cache_functions.cpp

# Removes all object files and the executable named main,
# so we can start fresh
clean:
	rm -f *.o main
