# Makefile
#
# It uses implicit rules to compile .c files using the
# values assigned to macros CC and CFLAGS
#

CC = gcc
CFLAGS = -g -Wall -std=c17
#CFLAGS += -fsanitize=thread
TARGETS = shared

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS)
