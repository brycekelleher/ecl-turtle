# Basic Makefile for the turtle package.

CC = clang
CFLAGS = -ggdb `ecl-config --cflags`
LDLIBS = -lm `ecl-config --ldlibs`
LDFLAGS = `ecl-config --ldflags`

.PHONY: clean build run

build: turtle-bin

clean:
	rm -f turtle turtle.o

run: turtle-bin turtle.sh
	./turtle.sh

turtle-bin: turtle.o
	cc $< -o $@ $(LIBS) $(LDFLAGS)

turtle.o: turtle.c
	cc -c $< -o $@ $(CFLAGS)
