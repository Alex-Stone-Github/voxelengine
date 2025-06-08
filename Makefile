CC = g++
CFLAGS = -std=c++23 -Wall -lSDL2 -lGLEW -lGL

sources = main.cpp shader.cpp camera.cpp chunk.cpp vector3.cpp texture.cpp world.cpp gizmo.cpp network.cpp
out = main

.PHONY: linux linux-bin clean run runs 

linux: linux-bin
	-mkdir build
	-cp $(out) build
	-cp shader build -r
	-cp picture build -r
	-cp doc build -r
	-cp README.md build
	-cp bootstrap-linux.sh build
	-cp voxelserver/target/debug/voxelserver build
linux-bin:
	$(CC) $(sources) -o $(out) $(CFLAGS)

run: all
	./$(out)

clean:
	-rm $(out)
	-rm build
