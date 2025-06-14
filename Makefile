# Build Flags and plateform specific flags
CC = g++
UNIXLIBS = -lGLEW -lGL
WINLIBS = -lglew32 -lopengl32 -DGLEW_STATIC -lws2_32 -DWIN -DSDL_MAIN_HANDLED -static-libgcc -static-libstdc++
CFLAGS = -std=c++23 -Wall -lSDL2 -I./vender -DGLM_ENABLE_EXPERIMENTAL #-Wextra
DBG = -g

# File Names
SOURCES = main.cpp shader.cpp camera.cpp chunk.cpp vector3.cpp texture.cpp world.cpp gizmo.cpp network.cpp netplat.cpp
BUILDDIRNAME = build
OUT = main
RELEASENAME = $(BUILDDIRNAME)-$(OUT)-all.zip

.PHONY: all clean release

$(BUILDDIRNAME): $(BUILDDIRNAME)/$(OUT) $(BUILDDIRNAME)/$(OUT).exe
	-mkdir $(BUILDDIRNAME)
	-cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/* $(BUILDDIRNAME)
	-cp voxelserver/target/debug/voxelserver $(BUILDDIRNAME)
	-cp bootstrap-linux.sh $(BUILDDIRNAME)
	-cp $(OUT)* $(BUILDDIRNAME)
	-cp shader $(BUILDDIRNAME) -r
	-cp picture $(BUILDDIRNAME) -r
	-cp doc $(BUILDDIRNAME) -r
	-cp README.md $(BUILDDIRNAME)
$(BUILDDIRNAME)/$(OUT): $(SOURCES) $(BUILDDIR)
	-@mkdir $(BUILDDIRNAME)
	$(CC) $^ -o $@ $(CFLAGS) $(DBG) $(UNIXLIBS)
$(BUILDDIRNAME)/$(OUT).exe: $(SOURCES) $(BUILDDIR) # Only one supportoed compiler for windows
	-@mkdir $(BUILDDIRNAME)
	x86_64-w64-mingw32-g++ $^ -o $@ $(CFLAGS) $(DBG) $(WINLIBS)
release: $(BUILDDIRNAME)
	zip $(RELEASENAME) $(BUILDDIRNAME) -r
clean:
	-rm $(BUILDDIRNAME) -rf
	-rm $(RELEASENAME) -rf
