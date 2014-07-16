# Makefile for graphics tests.

SOURCE_ROOT =../../src

CXX = $(COMPILER)
CXXFLAGS = -g -Wall -Wextra -O2 -std=c++11
CPPFLAGS = -I$(SOURCE_ROOT) -I$(SOURCE_ROOT)/input_management

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



all: main

debug: CPPFLAGS += -DDEBUG
debug: all



main: main.o game_window.o input_manager.o keyboard_input_event.o input_event.o lifeline.o lifeline_controller.o 
	$(CXX) -o main main.o game_window.o input_manager.o keyboard_input_event.o input_event.o lifeline.o lifeline_controller.o $(CXXFLAGS) $(SDL_LDLIBS) $(GRAPHICS_LDFLAGS) $(GRAPHICS_LDLIBS) $(X11_LDLIBS)
main.o: main.cpp
	$(CXX) -c -o main.o main.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS)


game_window.o: $(SOURCE_ROOT)/game_window.cpp
	$(CXX) -c -o game_window.o $(SOURCE_ROOT)/game_window.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS)

input_manager.o: $(SOURCE_ROOT)/input_management/input_manager.cpp
	$(CXX) -c -o input_manager.o $(SOURCE_ROOT)/input_management/input_manager.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS)

# lifeline.o: $(SOURCE_ROOT)/lifeline.cpp
# 	$(CXX) -c -o lifeline.o $(SOURCE_ROOT)/lifeline.cpp $(CXXFLAGS) $(CPPFLAGS)

input_event.o: $(SOURCE_ROOT)/input_management/input_event.cpp
	$(CXX) -c -o input_event.o $(SOURCE_ROOT)/input_management/input_event.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS)

keyboard_input_event.o: $(SOURCE_ROOT)/input_management/keyboard_input_event.cpp
	$(CXX) -c -o keyboard_input_event.o $(SOURCE_ROOT)/input_management/keyboard_input_event.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS)

lifeline.o: $(SOURCE_ROOT)/lifeline.cpp
	$(CXX) -c -o lifeline.o $(SOURCE_ROOT)/lifeline.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS)

lifeline_controller.o: $(SOURCE_ROOT)/lifeline_controller.cpp
	$(CXX) -c -o lifeline_controller.o $(SOURCE_ROOT)/lifeline_controller.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS)



clean:
	- rm -f main
	- rm -f main.o
	- rm -f game_window.o
	- rm -f input_manager.o
	- rm -f keyboard_input_event.o
	- rm -f lifeline.o
	- rm -f lifeline_controller.o
