CC = gcc
CFLAGS = -g -Wall -pedantic
OUT = test
OBJS = linked.o tests.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT)


