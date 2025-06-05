CC = g++
sources = main.cpp shader.cpp camera.cpp chunk.cpp vector3.cpp texture.cpp world.cpp gizmo.cpp network.cpp netplat.cpp
out = main

CFLAGS = -std=c++23 -Wall -lSDL2 -I./vender
WINONLY = -DWIN -I./vender/win64/glew-2.1.0/include -L./vender/win64/glew-2.1.0/lib/Release/x64 -I./vender/win64/SDL2-2.32.8/x86_64-w64-mingw32/include -L./vender/win64/SDL2-2.32.8/x86_64-w64-mingw32/lib -lglew32 -lopengl32 -lmingw32 -lstdc++
LINUXONLY = -lGL -lGLEW

.PHONY: all clean run runs windows

all:
	$(CC) $(sources) -o $(out) $(CFLAGS) $(LINUXONLY)

windows:
	x86_64-w64-mingw32-g++ $(sources) -o $(out) $(CFLAGS) $(WINONLY)

run: all
	./$(out)

clean:
	rm $(out)
