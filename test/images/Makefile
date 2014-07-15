# Makefile for graphics tests.

SOURCE_ROOT =../../src

CXX = $(COMPILER)
CXXFLAGS = -g -Wall -Wextra -O0 -std=c++11
CPPFLAGS = -I$(SOURCE_ROOT)

SDL_CFLAGS    = $(shell sdl2-config --cflags)
SDL_LDLIBS    = $(shell sdl2-config --libs) -lSDL2_image

# Taken from hello_triangle example
GLES_CFLAGS   = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads
GLES_LDFLAGS  = -L/opt/vc/lib/
GLES_LDLIBS   = -lGLESv2 -lEGL -lbcm_host -lpthread

GL_CFLAGS   = $(shell pkg-config gl --cflags)
GL_LDFLAGS  = 
GL_LDLIBS   = $(shell pkg-config gl --libs)

X11_CFLAGS = $(shell pkg-config x11 --cflags)
X11_LDLIBS = $(shell pkg-config x11 --libs)

ifeq ($(PLATFORM),desktop)
	GRAPHICS_CFLAGS  = $(GL_CFLAGS)
	GRAPHICS_LDFLAGS = $(GL_LDFLAGS)
	GRAPHICS_LDLIBS  = $(GL_LDLIBS)
	CPPFLAGS += -DUSE_GL
else
	GRAPHICS_CFLAGS  = $(GLES_CFLAGS)
	GRAPHICS_LDFLAGS = $(GLES_LDFLAGS)
	GRAPHICS_LDLIBS  = $(GLES_LDLIBS)
	CPPFLAGS += -DUSE_GLES
endif



all: main

debug: CPPFLAGS += -DDEBUG
debug: all



main: main.o image.o 
	$(CXX) -o main main.o image.o $(CXXFLAGS) $(SDL_LDLIBS) $(GRAPHICS_LDFLAGS) $(GRAPHICS_LDLIBS) $(X11_LDLIBS)
main.o: main.cpp
	$(CXX) -c -o main.o main.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS)


image.o: $(SOURCE_ROOT)/image.cpp
	$(CXX) -c -o image.o $(SOURCE_ROOT)/image.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS)



clean:
	- rm -f main
	- rm -f main.o
	- rm -f image.o
