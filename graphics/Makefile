# Makefile for graphics tests.

CXX = $(COMPILER)
CXXFLAGS = -g -Wall -Wextra -O2 -std=c++11
CPPFLAGS = -I../src

SDL_CFLAGS    = $(shell sdl2-config --cflags)
SDL_LDLIBS    = $(shell sdl2-config --libs)

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



all: game_window_test

debug: CPPFLAGS += -DDEBUG
debug: all



game_window_test: game_window_test.o game_window.o input_manager.o
	$(CXX) -o game_window_test game_window.o game_window_test.o input_manager.o $(CXXFLAGS) $(SDL_LDLIBS) $(GRAPHICS_LDFLAGS) $(GRAPHICS_LDLIBS) $(X11_LDLIBS)
game_window_test.o: game_window_test.cpp
	$(CXX) -c -o game_window_test.o game_window_test.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS)


game_window.o: ../src/game_window.cpp
	$(CXX) -c -o game_window.o ../src/game_window.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS)

input_manager.o: ../src/game_window.cpp
	$(CXX) -c -o input_manager.o ../src/input_manager.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS)



clean:
	- rm -f game_window_test
	- rm -f game_window_test.o
	- rm -f game_window.o
