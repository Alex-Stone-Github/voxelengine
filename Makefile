CC = g++
CFLAGS = -std=c++23 -Wall -lSDL2 -lGL

.PHONY: all clean

all:
	$(CC) main.cpp -o main $(CFLAGS)

clean:
	rm main