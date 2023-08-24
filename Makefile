# Define C compiler & flags
CC = gcc 
CFLAGS = -Wall -g
# Define libraries to be linked (for example -lm)
LDLIBS = -lm

# Define sets of source files and object files
SRC = allocate.c linkedlist.c shortestjobfirst.c roundrobin.c memory.c process.c
# OBJ is the same as SRC, just replace .c with .o
OBJ = $(SRC:.c=.o)
 
# Define the executable name(s)
EXE = allocate process

all : allocate process

allocate: allocate.o linkedlist.o shortestjobfirst.o roundrobin.o memory.o
	gcc -Wall -o allocate allocate.o linkedlist.o shortestjobfirst.o roundrobin.o memory.o 

process: process.o
	gcc -Wall -o process process.o

# Other targets specify how to get the .o files
allocate.o: allocate.c 
	gcc -Wall -c allocate.c 

linkedlist.o: linkedlist.c linkedlist.h
	gcc -Wall -c linkedlist.c

shortestjobfirst.o : shortestjobfirst.c shortestjobfirst.h
	gcc -Wall -c shortestjobfirst.c

roundrobin.o : roundrobin.c roundrobin.h
	gcc -Wall -c roundrobin.c

memory.o : memory.c memory.h
	gcc -Wall -c memory.c

process.o : process.c
	gcc -Wall -c process.c

clean:
	rm -f *.o allocate process
