CC = g++
UNIXLIBS = -lGLEW -lGL
WINLIBS = -lglew32 -lopengl32 -DGLEW_STATIC
CFLAGS = -std=c++23 -Wall -lSDL2 -I./vender -DGLM_ENABLE_EXPERIMENTAL #-Wextra
DBG = -g

sources = main.cpp shader.cpp camera.cpp chunk.cpp vector3.cpp texture.cpp world.cpp gizmo.cpp network.cpp netplat.cpp
out = main

.PHONY: linux linux-bin windows-bin clean run

linux: linux-bin
	-mkdir build
	-cp $(out) build
	-cp shader build -r
	-cp picture build -r
	-cp doc build -r
	-cp README.md build
	-cp bootstrap-linux.sh build
	-cp voxelserver/target/debug/voxelserver build
windows: windows-bin
	-mkdir build
	-cp $(out).exe build
	-cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/* build
	-cp shader build -r
	-cp picture build -r
	-cp doc build -r
	-cp README.md build
	-cp voxelserver/target/debug/voxelserver build
linux-bin:
	$(CC) $(sources) -o $(out) $(CFLAGS) $(DBG) $(UNIXLIBS)
windows-bin:
# Only one supportoed compiler for windows
	x86_64-w64-mingw32-g++ $(sources) -o $(out) $(CFLAGS) $(DBG) $(WINLIBS) -DWIN -DSDL_MAIN_HANDLED -static-libgcc -static-libstdc++

run: all
	./$(out)

clean:
	-rm $(out)
	-rm $(out).exe
	-rm build -rf
