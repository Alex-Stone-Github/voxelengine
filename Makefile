CC = g++
sources = main.cpp shader.cpp camera.cpp chunk.cpp vector3.cpp texture.cpp world.cpp gizmo.cpp network.cpp netplat.cpp
out = main

CFLAGS = -std=c++23 -Wall -lSDL2 -I./vender
WINONLY = -DWIN -I./vender/win64/glew-2.1.0/include -L./vender/win64/glew-2.1.0/lib/Release/x64 -I./vender/win64/SDL2-2.32.8/x86_64-w64-mingw32/include -L./vender/win64/SDL2-2.32.8/x86_64-w64-mingw32/lib -lglew32 -lopengl64 -lmingw32 -static-libgcc -static-libstdc++
LINUXONLY = -lGL -lGLEW

.PHONY: linux-bin clean run runs windows windows-bin linux

linux: linux-bin
	-mkdir build
	-cp $(out) build
	-cp shader build -r
	-cp picture build -r
	-cp README.md build
	-cp bootstrap-linux.sh build
	-cp voxelserver/target/debug/voxelserver build
linux-bin:
	$(CC) $(sources) -o $(out) $(CFLAGS) $(LINUXONLY)

windows: windows-bin
	-mkdir build
	-cp $(out).exe build
	-cp vender/dlls/* build
	-cp vender/win64/glew-2.1.0/bin/Release/x64/glew32.dll build
	-cp vender/win64/SDL2-2.32.8/x86_64-w64-mingw32/bin/SDL2.dll build
	-cp shader build -r
	-cp picture build -r
	-cp README.md build
windows-bin:
	x86_64-w64-mingw32-g++ $(sources) -o $(out) $(CFLAGS) $(WINONLY)

clean:
	-rm $(out)
	-rm $(out).exe
	-rm build -rf
