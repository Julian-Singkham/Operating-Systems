CC=gcc
CFLAGS=-Wall -g -lrt

BINS=$(patsubst %.c,%,$(wildcard *.c))

all: $(BINS)

clean:
	rm -f $(BINS) MYFILE

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
