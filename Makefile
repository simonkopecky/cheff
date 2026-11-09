CC=g++
CFLAGS=-Wall -Wextra -pedantic

build:
	$(CC) $(CFLAGS) main.cpp -o cheff

all: build

run: build
	./cheff
