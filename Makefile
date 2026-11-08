CC=gcc
CFLAGS=-Wall -Wextra -pedantic

build:
	$(CC) $(CFLAGS) main.c -o cheff

all: build

run: build
	./cheff
