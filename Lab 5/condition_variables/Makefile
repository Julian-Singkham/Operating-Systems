CC=gcc
CFLAGS=-O0 -Wall -g -pthread

BINS=$(patsubst prod_con%.c,prod_con%,$(wildcard prod_con*.c))

all: $(BINS)

clean:
	rm -f $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
