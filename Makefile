CC = g++
CFLAGS = -std=c++23 -Wall -lSDL2 -lGLEW -lGL

sources = main.cpp shader.cpp camera.cpp chunk.cpp vector3.cpp texture.cpp
out = main

.PHONY: all clean run

all:
	$(CC) $(sources) -o $(out) $(CFLAGS)

run: all
	./main

clean:
	rm main