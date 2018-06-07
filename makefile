CC = gcc
CFLAGS = -Wall  -O2 -pedantic -g
OUT = test
OBJS = linked.o tests.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT)


