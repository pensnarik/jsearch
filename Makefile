CC=gcc
CFLAGS=-O2
LDFLAGS=

all: jsearch

jsearch: jsearch.c
	$(CC) $(CFLAGS) jsearch.c -o jsearch

clean:
	rm ./jsearch
