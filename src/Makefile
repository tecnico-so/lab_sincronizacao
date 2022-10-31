# Makefile
#
# It uses implicit rules to compile .c files using the
# values assigned to macros CC and CFLAGS
#

CC = gcc
CFLAGS = -g -Wall -std=c17 #-fsanitize=thread
TARGETS = contapartilhada

.PHONY: all clean contapartilhada.o contapartilhada

contapartilhada: contapartilhada.o
	$(CC) $(CFLAGS) contapartilhada.o -o contapartilhada -lpthread

contapartilhada.o: contapartilhada.c
	$(CC) $(CFLAGS) -c contapartilhada.c -o contapartilhada.o -lpthread

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS) 