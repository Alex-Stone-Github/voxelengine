# Build Flags and plateform specific flags
CC = g++
UNIXLIBS = -lGLEW -lGL
WINLIBS = -lglew32 -lopengl32 -DGLEW_STATIC -lws2_32 -DWIN -DSDL_MAIN_HANDLED -static-libgcc -static-libstdc++
CFLAGS = -std=c++23 -Wall -lSDL2 -I./vender -DGLM_ENABLE_EXPERIMENTAL -Wextra
DBG = -g

# File Names
SOURCES = main.cpp shader.cpp camera.cpp chunk.cpp vector3.cpp texture.cpp world.cpp gizmo.cpp network.cpp netplat.cpp
BUILDDIRNAME = build
OUT = voxelclient
RELEASENAME = $(BUILDDIRNAME)-$(OUT)-all.zip

.PHONY: all clean release $(BUILDDIRNAME)

all: $(BUILDDIRNAME)/$(OUT) $(BUILDDIRNAME)/$(OUT).exe $(BUILDDIRNAME)/voxelserver $(BUILDDIRNAME)/voxelserver.exe 
	-@mkdir $(BUILDDIRNAME) -p
	cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/* $(BUILDDIRNAME)
	cp shader $(BUILDDIRNAME) -r
	cp picture $(BUILDDIRNAME) -r
	cp bootstrap-linux.sh $(BUILDDIRNAME)

	cp doc $(BUILDDIRNAME) -r
	cp README.md $(BUILDDIRNAME)
$(BUILDDIRNAME)/voxelserver.exe:
	-@mkdir $(BUILDDIRNAME) -p
	@echo "Building Voxel Server(WIN64)!"
	@cargo build --manifest-path=voxelserver/Cargo.toml --target=x86_64-pc-windows-gnu 
	cp voxelserver/target/x86_64-pc-windows-gnu/debug/voxelserver.exe $(BUILDDIRNAME)
$(BUILDDIRNAME)/voxelserver:
	-@mkdir $(BUILDDIRNAME) -p
	@echo "Building Voxel Server!"
	@cargo build --manifest-path=voxelserver/Cargo.toml
	cp voxelserver/target/debug/voxelserver $(BUILDDIRNAME)
$(BUILDDIRNAME)/$(OUT): $(SOURCES)
	-@mkdir $(BUILDDIRNAME) -p
	$(CC) $(SOURCES) -o $@ $(CFLAGS) $(DBG) $(UNIXLIBS)
$(BUILDDIRNAME)/$(OUT).exe: $(SOURCES) # Only one supportoed compiler for windows
	-@mkdir $(BUILDDIRNAME) -p
	x86_64-w64-mingw32-g++ $(SOURCES) -o $@ $(CFLAGS) $(DBG) $(WINLIBS)
$(RELEASENAME): all
	zip $(RELEASENAME) $(BUILDDIRNAME) -r
release: $(RELEASENAME)
clean:
	-rm $(BUILDDIRNAME) -rf
	-rm $(RELEASENAME) -rf
