# Makefile
#
# It uses implicit rules to compile .c files using the
# values assigned to macros CC and CFLAGS
#

CC = gcc
CFLAGS = -g -Wall -std=c17
#CFLAGS += -fsanitize=thread

# enable all POSIX APIs (required in some systems)
CFLAGS += -D_POSIX_C_SOURCE=200809L

TARGETS = shared

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS)
