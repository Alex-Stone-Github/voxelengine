CC = g++
CFLAGS = -std=c++23 -Wall -lSDL2 -lGLEW -lGL

sources = main.cpp shader.cpp
out = main

.PHONY: all clean

all:
	$(CC) $(sources) -o $(out) $(CFLAGS)

clean:
	rm main